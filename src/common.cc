#include "cpplogo/common.h"

namespace cpplogo {

/***********************************************************
* std_to_boost
* Convert a STL vector of doubles to a boost vector of
* doubles.
***********************************************************/
vectord std_to_boost(std::vector<double> std)
{
  vectord bv(std.size());
  for (size_t i = 0; i < bv.size(); i++) {
    bv(i) = std[i];
  }
  return bv;
} /* std_to_boost() */

}
