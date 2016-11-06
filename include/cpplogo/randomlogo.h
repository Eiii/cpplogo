/***********************************************************
* randomlogo.h
* Implements LOGO with random split dimension selection.
***********************************************************/
#pragma once
#include "cpplogo/logo.h"
#include "cpplogo/randomsoo.h"

namespace cpplogo {

class RandomLOGO : public RandomSOO, public LOGO {
/***********************************************************
* Options structure
***********************************************************/
  public:
    struct Options : public RandomSOO::Options, public LOGO::Options {
      Options(ObjectiveFn fn, int dim, int max_observations, 
              int num_children, int seed, std::vector<int> w_schedule) :
        SOO::Options(fn, dim, max_observations, num_children),
        RandomSOO::Options(fn, dim, max_observations, num_children, seed),
        LOGO::Options(fn, dim, max_observations, num_children, w_schedule)
      {}
    };

  public:
    RandomLOGO(const Options& opt) :
      SOO(opt), RandomSOO(opt), LOGO(opt) {}
    virtual ~RandomLOGO() = default;
};

}
