#include "cpplogo/optintf.h"

namespace cpplogo {

/***********************************************************
* OptIntf constructor
***********************************************************/
OptIntf::OptIntf(const Options& opt) :
  fn_(opt.fn), dim_(opt.dim), max_observations_(opt.max_observations), 
  num_observations_(0) 
{
} /* OptIntf() */

/***********************************************************
* Optimize
* Keep executing optimization steps until finished.
***********************************************************/
void OptIntf::Optimize() 
{
  while (!IsFinished()) {
    Step();
  }
} /* Optimize() */

/***********************************************************
* Step
* Execute a single step of the optimization procudure.
***********************************************************/
void OptIntf::Step() {
  BeginStep();

  // For each 'depth level', expand the best node at that
  // depth.
  for (int depth = 0; depth <= CalculateMaxDepth(); depth++) {
    ExpandBestAtDepth(depth);
  }

  EndStep();
} /* Step() */

/***********************************************************
* IsFinished
* Returns true if the optiization is finished.
***********************************************************/
bool OptIntf::IsFinished() const {
  return num_observations_ >= max_observations_;
}

}
