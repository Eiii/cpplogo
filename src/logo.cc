#include "cpplogo/logo.h"
#include "cpplogo/logging.h"

namespace cpplogo {

/***********************************************************
* LOGO constructor
***********************************************************/
LOGO::LOGO(const Options& opt) :
  SOO(opt), w_schedule_(opt.w_schedule), depthset_width_(w_schedule_[0]),
  last_best_obs_(-std::numeric_limits<double>::infinity())
{
} /* LOGO() */

/***********************************************************
* EndStep
* Executed at the end of each optimization step.
***********************************************************/
void LOGO::EndStep()
{
  // Find the best node that was observed in this step
  auto comp = [](const Node a, const Node b){return a.value() < b.value();};
  const auto step_best_node = std::max_element(step_observed_nodes_.begin(), 
                                               step_observed_nodes_.end(), 
                                               comp);
  double step_best = (*step_best_node).value();
  LOG(trace) << "Previous best: " << last_best_obs_ << ", step best: " << step_best;

  // Update the W value based on the value of the best node observed in this
  // step and the value of the best node observed in the previous step
  size_t next_idx;
  // Find the index of the W value we're at now
  auto current_it = std::find(w_schedule_.begin(), w_schedule_.end(), 
                              depthset_width_);
  size_t current_idx = std::distance(w_schedule_.begin(), current_it);
  // Figure out what the index of the next W value should be
  if (step_best > last_best_obs_) {
    next_idx = current_idx+1;
  } else {
    next_idx = current_idx-1;
  }
  // Clamp it to the size of the W schedule
  next_idx = std::min(next_idx, w_schedule_.size()-1);
  next_idx = std::max(next_idx, (size_t)0);
  // Set the width (w) for the next step
  depthset_width_ = w_schedule_[next_idx];

  LOG(trace) << "Width is now: " << depthset_width_;
  last_best_obs_ = step_best;
} /* EndStep() */

/***********************************************************
* CalculateMaxDepth
* Calculate the maximum depth for this algorithm.
***********************************************************/
size_t LOGO::CalculateMaxDepth() const
{
  size_t soo_depth = SOO::CalculateMaxDepth();
  return soo_depth / depthset_width_;
} /* CalculateMaxDepth() */

/***********************************************************
* BestNodeAtDepth
* Find the best node at the specified 'depth' level.
* NOTE: In this case, a single 'depth' level encompasses
* multiple depth levels in the space, according to the 
* width.
***********************************************************/
const Node* LOGO::BestNodeAtDepth(size_t depthset_id) const
{
  // Find the minimum/maximum 'true' depth levels for this depth index
  size_t min_depth = depthset_id*depthset_width_;
  size_t max_depth = min_depth+depthset_width_-1;
  LOG(trace) << "Considering nodes from " << min_depth << "-" << max_depth;

  // Find the best node within those depths
  const Node* best_node = nullptr;
  for (size_t d = min_depth; d <= max_depth; d++) {
    const Node* depth_best = SOO::BestNodeAtDepth(d);   
    if (depth_best == nullptr) {
      continue;
    }
    if (best_node == nullptr || depth_best->value() > best_node->value()) {
      best_node = depth_best;
    }
  }
  return best_node;
} /* BestNodeAtDepth() */

}

