#include "cpplogo/randomsoo.h"
#include "cpplogo/logging.h"

namespace cpplogo {

/***********************************************************
* RandomSOO constructor
***********************************************************/
RandomSOO::RandomSOO(const Options& opt) :
  SOO(opt), seed_(opt.seed), rng_(seed_)  
{
} /* RandomSOO() */

/***********************************************************
* ChooseSplitDimension
* Find all the nodes' dimension with maximum size, and 
* randomly choose one.
***********************************************************/
size_t RandomSOO::ChooseSplitDimension(const Node* node) const
{
  double max_size = -std::numeric_limits<double>::infinity();
  std::vector<size_t> possible_dim;
  for (size_t d = 0; d < static_cast<size_t>(dim_); d++) {
    double size = node->sizes()[d];
    if (size > max_size) {
      possible_dim.clear();
      max_size = size;
    }
    if (size == max_size) {
      possible_dim.push_back(d);
    }
  }

  // Choose one of the valid dimensions at random (if there
  // are more than one)
  size_t dim;
  if (possible_dim.size() > 1) {
    std::uniform_int_distribution<size_t> dist(0, possible_dim.size()-1);
    dim = dist(rng_);
  } else {
    dim = possible_dim[0];
  }
  LOG(trace) << "Splitting along " << dim;
  return dim;
} /* ChooseSplitDimension() */

}
