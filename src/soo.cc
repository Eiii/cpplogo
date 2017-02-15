#include "cpplogo/soo.h"

#include "cpplogo/logging.h"
#include <algorithm>

using std::vector;

namespace cpplogo {

/***********************************************************
* SOO constructor
***********************************************************/
SOO::SOO(const Options& opt) :
  OptIntf(opt),
  vmax_(),
  num_expansions_(1),
  num_node_evals_(1),
  step_observed_nodes_()
{ 
} /* SOO() */

/***********************************************************
* BeginStep
* Called at the beginning of each optimization step.
***********************************************************/
void SOO::BeginStep() 
{
  LOG(trace) << "Beginning step";
  vmax_ = -std::numeric_limits<double>::infinity();
  step_observed_nodes_.clear();
} /* BeginStep() */

/***********************************************************
* EndStep
* Called at the end of each optimization step.
***********************************************************/
void SOO::EndStep()
{
  LOG(trace) << "Ending step";
} /* EndStep() */

/***********************************************************
* CalculateMaxDepth
* TODO: This should probably return a size_t
***********************************************************/
size_t SOO::CalculateMaxDepth() const
{
  size_t max_depth = std::sqrt(num_expansions_);
  max_depth = std::min(max_depth, space_.size());
  LOG(trace) << "Max depth = " << max_depth;
  return max_depth;
} /* CalculateMaxDepth() */

/***********************************************************
* StepExpandNodes
***********************************************************/
void SOO::StepExpandNodes()
{
  // For each 'depth level', expand the best node at that
  // depth.
  for (size_t depth = 0; depth <= CalculateMaxDepth(); depth++) {
    ExpandBestAtDepth(depth);
  }
} /* StepExpandNodes() */

/***********************************************************
* ExpandBestAtDepth
* Expand the node with the best value at the specified
* depth.
***********************************************************/
void SOO::ExpandBestAtDepth(size_t depth)
{
  LOG(trace) << "Expanding depth " << depth;
  // Find the best node
  Node* best_node = BestNodeAtDepth(depth);
  if (!best_node) {
    LOG(trace) << "Depth empty";
    return;
  }
  LOG(trace) << "Best node = " << *best_node;

  // Expand the node and evaluate its children if the best node is good enough
  if (best_node->value() > vmax_) {
    vmax_ = best_node->value();
    LOG(trace) << "New vmax = " << vmax_;

    // Get the resultant children from the expansion
    vector<Node> children = ExpandNode(best_node);
    num_expansions_ += 1; 
    // Make observations for each un-observed node among the children
    ObserveNodes(&children);
    // Add children to space
    AddNodes(children);
    // Delete the node that was expanded
    RemoveNode(best_node);
  }
  num_node_evals_ += 1;
} /* ExpandBestAtDepth() */

/***********************************************************
* BestNodeAtDepth
* Find the node with the best value at the specified depth
* level.
* Returns a pointer to that node. Returns a null pointer
* if no nodes exist at the requested depth.
***********************************************************/
const Node* SOO::BestNodeAtDepth(size_t depth) const
{
  // If the depth doesn't exist, it can't have a best node
  if (depth >= space_.size()) return nullptr;
  const vector<Node>& nodes = space_.at(depth);
  const Node* best_node = nullptr;

  // Node comparison lambda function
  // See the max_element STL documentation
  auto node_comp = [](const Node& a, const Node& b) { 
    return a.value() < b.value(); 
  };
  auto best_it = std::max_element(nodes.begin(), nodes.end(), node_comp);

  // If we found a node, set the best_node pointer to the address of
  // the best node found
  // Otherwise, no nodes are at this depth and best_node will be null
  if (best_it != nodes.end()) {
    best_node = &(*best_it);
  }

  return best_node;
} /* BestNodeAtDepth() */

/***********************************************************
* BestNodeAtDepth
* Do some fancy casting to return a non-const pointer to the
* best node at the requested depth.
***********************************************************/
Node* SOO::BestNodeAtDepth(size_t depth) 
{
  const auto cthis = const_cast<const SOO*>(this);
  return const_cast<Node*>(cthis->BestNodeAtDepth(depth));
} /* BestNodeAtDepth() */

/***********************************************************
* ChooseSplitDimension
* Return which dimension the specified node should be split
* along.
* Effectively returns the first dimension with the maximum
* size.
***********************************************************/
size_t SOO::ChooseSplitDimension(const Node* node) const
{
  size_t max_dim = 0;
  for (size_t d = 0; d < static_cast<size_t>(dim_); d++) {
    if (node->sizes()[d] > node->sizes()[max_dim]) {
      max_dim = d;
    }
  }
  LOG(trace) << "Splitting along " << max_dim;
  return max_dim;
} /* ChooseSplitDimension() */

/***********************************************************
* ObserveNode
* Observe a value for the specified node.
***********************************************************/
bool SOO::ObserveNode(Node* node)
{
  bool result = OptIntf::ObserveNode(node);

  // We need to copy the node here, otherwise it might
  // get deleted in a future expansion during this step!
  step_observed_nodes_.push_back(*node);

  return result;
} /* ObserveNode() */

}
