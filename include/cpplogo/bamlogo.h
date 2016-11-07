/***********************************************************
* bamlogo.h
* Implements the LOGO algorithm with BaMSOO's GP extension.
***********************************************************/
#pragma once
#include "cpplogo/logo.h"
#include "cpplogo/bamsoo.h"

namespace cpplogo {

class BaMLOGO : public LOGO, public BaMSOO {
/***********************************************************
* Options structure
***********************************************************/
  public:
    struct Options : public LOGO::Options, public BaMSOO::Options {
      Options(ObjectiveFn fn, int dim, int max_observations, 
              int num_children, std::vector<int> w_schedule) :
        SOO::Options(fn, dim, max_observations, num_children),
        LOGO::Options(fn, dim, max_observations, num_children, w_schedule),
        BaMSOO::Options(fn, dim, max_observations, num_children)
      {}
    };

  public:
    BaMLOGO(const Options& opt) :
      SOO(opt), LOGO(opt), BaMSOO(opt) {}
    virtual ~BaMLOGO() = default;
};

}
