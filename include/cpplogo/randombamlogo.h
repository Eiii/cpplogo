/***********************************************************
* randombamlogo.h
* Implements BaMLOGO with random split dimension selection.
***********************************************************/
#pragma once
#include "cpplogo/bamlogo.h"
#include "cpplogo/randomsoo.h"

namespace cpplogo {

class RandomBaMLOGO : public RandomSOO, public BaMLOGO {
/***********************************************************
* Options structure
***********************************************************/
  public:
    struct Options : public RandomSOO::Options, public BaMLOGO::Options {
      Options(ObjectiveFn fn, int dim, int max_observations, 
              int num_children, int seed, std::vector<int> w_schedule) :
        SOO::Options(fn, dim, max_observations, num_children),
        RandomSOO::Options(fn, dim, max_observations, num_children, seed),
        BaMLOGO::Options(fn, dim, max_observations, num_children, w_schedule)
      {}
    };

  public:
    RandomBaMLOGO(const Options& opt) :
      SOO(opt), RandomSOO(opt), BaMLOGO(opt) {}
    virtual ~RandomBaMLOGO() = default;
};

}
