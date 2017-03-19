/***********************************************************
* randomimgpo.h
* Implements IMGPO with random split dimension selection.
***********************************************************/
#pragma once
#include "cpplogo/imgpo.h"
#include "cpplogo/randomsoo.h"

namespace cpplogo {

class RandomIMGPO : public RandomSOO, public IMGPO {
/***********************************************************
* Options structure
***********************************************************/
  public:
    struct Options : public RandomSOO::Options, public IMGPO::Options {
      Options(ObjectiveFn fn, int dim, int max_observations, 
              int num_children, int seed, int max_depth) :
        SOO::Options(fn, dim, max_observations, num_children),
        BaMSOO::Options(fn, dim, max_observations, num_children),
        RandomSOO::Options(fn, dim, max_observations, num_children, seed),
        IMGPO::Options(fn, dim, max_observations, num_children, max_depth)
      {}
    };

  public:
    RandomIMGPO(const Options& opt) :
      SOO(opt), BaMSOO(opt), RandomSOO(opt), IMGPO(opt) {}
    virtual ~RandomIMGPO() = default;
};

}
