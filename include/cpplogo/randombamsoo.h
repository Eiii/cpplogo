/***********************************************************
* randombamsoo.h
* Implements BaMSOO with random split dimension selection.
***********************************************************/
#pragma once
#include "cpplogo/bamsoo.h"
#include "cpplogo/randomsoo.h"

namespace cpplogo {

class RandomBaMSOO : public RandomSOO, public BaMSOO {
/***********************************************************
* Options structure
***********************************************************/
  public:
    struct Options : public RandomSOO::Options, public BaMSOO::Options {
      Options(ObjectiveFn fn, int dim, int max_observations, 
              int num_children, int seed) :
        SOO::Options(fn, dim, max_observations, num_children),
        RandomSOO::Options(fn, dim, max_observations, num_children, seed),
        BaMSOO::Options(fn, dim, max_observations, num_children)
      {}
    };

  public:
    RandomBaMSOO(const Options& opt) :
      SOO(opt), RandomSOO(opt), BaMSOO(opt) {}
    virtual ~RandomBaMSOO() = default;
};

}
