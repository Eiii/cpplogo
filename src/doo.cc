#include "cpplogo/doo.h"

#include "cpplogo/logging.h"
#include <algorithm>

using std::vector;

namespace cpplogo {

/***********************************************************
* DOO constructor
***********************************************************/
DOO::DOO(const Options& opt) :
  OptIntf(opt),
  max_slope_(opt.max_slope),
  step_observed_node_(nullptr)
{ 
} /* DOO() */

/***********************************************************
* BeginStep
* Called at the beginning of each optimization step.
***********************************************************/
void DOO::BeginStep() 
{
  LOG(trace) << "Beginning step";
  step_observed_node_.release();
} /* BeginStep() */

/***********************************************************
* EndStep
* Called at the end of each optimization step.
***********************************************************/
void DOO::EndStep()
{
  LOG(trace) << "Ending step";
} /* EndStep() */

/***********************************************************
* StepExpandNodes
***********************************************************/
void DOO::StepExpandNodes()
{
  Node* best_node = nullptr;
  double best_value = -std::numeric_limits<double>::infinity();
  for (auto& level : space_) {
    for (auto& node : level) {
      double ub = CalculateUpperBound(node);
      if (ub > best_value) {
        best_node = &node;
        best_value = ub;
      }
    }
  }
  assert(best_node);

  //Save off the node we're going to observe (and delete)
  step_observed_node_.reset(new Node(*best_node));

  auto children = ExpandNode(best_node);
  ObserveNodes(&children);
  AddNodes(children);
  RemoveNode(best_node);
} /* StepExpandNodes() */

/***********************************************************
* ChooseSplitDimension
* Return which dimension the specified node should be split
* along.
* Effectively returns the first dimension with the maximum
* size.
***********************************************************/
size_t DOO::ChooseSplitDimension(const Node* node) const
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
* CalculateUpperBound
* Use the function's known smoothness to calculate the 
* upper bound on the max value contained in a node.
***********************************************************/
double DOO::CalculateUpperBound(const Node& node)
{
  assert(node.has_value());
  double sqr_width = 0.0;
  for (size_t i = 0; i < node.sizes().size(); i++) {
    double half_dist = node.sizes()[i]/2;
    sqr_width += half_dist*half_dist;
  }
  double width = sqrt(sqr_width);

  double ub = node.value() + width*max_slope_;
  LOG(trace) << node;
  LOG(trace) << width;
  LOG(trace) << ub;
  return ub;
} /* CalculateUpperBound() */

}
