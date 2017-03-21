#include "cpplogo/imgpo.h"
#include "cpplogo/logging.h"

namespace cpplogo {

/***********************************************************
* IMGPO constructor
***********************************************************/
IMGPO::IMGPO(const Options& options) :
  SOO(options), BaMSOO(options), subtree_max_depth_(options.subtree_max_depth),
  current_subtree_depth_(1.0), prev_best_(std::numeric_limits<double>::quiet_NaN())
{
} /* IMGPO() */

/***********************************************************
* BeginStep
***********************************************************/
void IMGPO::BeginStep()
{
  BaMSOO::BeginStep();
  auto best = BestNode();
  if (best) prev_best_ = best->value();
} /* BeginStep() */

/***********************************************************
* EndStep
***********************************************************/
void IMGPO::EndStep()
{
  BaMSOO::EndStep();
  auto best = BestNode();
  bool increase_depth = std::isnan(prev_best_) || (best && (best->value() > prev_best_));
  if (increase_depth) {
    current_subtree_depth_ += 4;
  } else {
    current_subtree_depth_ = std::max(current_subtree_depth_-0.5, 4.0);
  }
  LOG(trace) << "New subtree depth: " << current_subtree_depth_;
} /* EndStep() */

/***********************************************************
* CalculateMaxDepth
***********************************************************/
size_t IMGPO::CalculateMaxDepth() const
{
  size_t max_depth = space_.size();
  LOG(trace) << "Max depth = " << max_depth;
  return max_depth;
} /* CalculateMaxDepth() */

/***********************************************************
* NodeShouldBeExpanded
***********************************************************/
bool IMGPO::NodeShouldBeExpanded(Node* node) 
{
  LOG(trace) << "Evaluating " << *node;  
  if (!BaMSOO::NodeShouldBeExpanded(node)) return false;
  if (node->is_fake_value()) SOO::ObserveNode(node);

  //This node has passed all the previous expansion tests,
  //so check if it contains a UCB greater than any smaller interval.
  
  //Calculate e
  int e = 0;
  int current_sd = static_cast<int>(std::ceil(current_subtree_depth_));
  const Node* best_smaller = nullptr;
  for (int edepth = 1; 
      edepth <= std::min(subtree_max_depth_, current_sd);
      edepth++) {
    const Node* best_at_depth = BestNodeAtDepth(node->depth()+edepth);
    if (best_at_depth && 
        best_at_depth->value() >= vmax_ && !best_at_depth->is_fake_value()) {
      LOG(trace) << "Found suitable smaller node at edepth " << edepth;  
      best_smaller = best_at_depth;
      e = edepth;
      break;
    }
  }

  if (e == 0) {
    LOG(trace) << "No suitable smaller node found, expanding.";
    return true;
  }

  double best_ucb = BestSubtreeUCB(node, e);
  LOG(trace) << "Best UCB: " << best_ucb;
  LOG(trace) << "Best Smaller: " << best_smaller->value();
  if (best_ucb < best_smaller->value()) {
    LOG(trace) << "Ignoring";
    return false;
  } else {
    LOG(trace) << "Best UCB: " << best_ucb;
    LOG(trace) << "Best Smaller: " << best_smaller->value();
    LOG(trace) << "Expanding" << num_observations_;
    return true;
  }
} /* NodeShouldBeExpanded() */

/***********************************************************
* BestSubtreeUCB
***********************************************************/
double IMGPO::BestSubtreeUCB(const Node* node, int depth)
{
  assert(depth > 0);
  assert(node->has_value());


  //Build the subtree to the appropriate depth.
  std::vector<Node> subtree = BuildSubtree(node, depth);

  //Calculate the UCB for each node in the subtree
  double best_ucb = -std::numeric_limits<double>::infinity();
  for (const auto& node : subtree) {
    double ucb;
    std::tie(std::ignore, ucb) = CalculateLCBUCB(&node);
    best_ucb = std::max(best_ucb, ucb);
  }

  return best_ucb;
}

std::vector<Node> IMGPO::BuildSubtree(const Node* node, int depth)
{
  std::vector<Node> subtree;
  std::vector<Node> level = {*node};
  for (int i = 0; i < depth; i++) {
    std::vector<Node> next_level;
    for (auto& n : level) {
      std::vector<Node> exp = SOO::ExpandNode(&n);
      next_level.insert(next_level.end(), exp.begin(), exp.end()); 
    }
    subtree.insert(subtree.end(), next_level.begin(), next_level.end());
    level = next_level;
  }
  return subtree;
} /* BuildSubtree() */

/***********************************************************
* CalculateLCBUCB
* Calculate the LCB and UCB according to section 3.3 in the
* paper.
***********************************************************/
std::tuple<double, double> IMGPO::CalculateLCBUCB(const Node* node)
{
  auto center = node->Center();
  bayesopt::ProbabilityDistribution* prediction = gp_.GetPrediction(center);
  double mean = prediction->getMean();
  double std = prediction->getStd();
  //TODO: num_node_evals should be replaced by M, ideally.
  double mult = std::sqrt(
                  2.0 * 
                  std::log(
                    M_PI*M_PI*num_node_evals_*num_node_evals_/(12.0*n_)
                  )
                );
  double ucb = mean + mult*std;
  double lcb = mean - mult*std;
  LOG(trace) << "Mean: " << mean;
  LOG(trace) << "STD:  " << std;
  LOG(trace) << "Mult: " << mult;
  return std::make_tuple(lcb, ucb);
} /* CalculateLCBUCB() */

}
