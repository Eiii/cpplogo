/***********************************************************
* initbamsoo.h
* Implements an extension to BaMSOO in which the GP is
* initialized with a set of previous observations.
* This implementation is inherently random since the 
* initializing points are randomly selected, but that's not
* necessary.
***********************************************************/
#pragma once
#include "cpplogo/randombamsoo.h"

namespace cpplogo {

class InitBaMSOO : public RandomBaMSOO {
/***********************************************************
* Options structure
***********************************************************/
  public:
    struct Options : public RandomBaMSOO::Options {
      Options(ObjectiveFn fn, int dim, int max_observations, 
              int num_children, int seed, int init_obs) :
        SOO::Options(fn, dim, max_observations, num_children),
        RandomBaMSOO::Options(fn, dim, max_observations, num_children, seed),
        init_obs(init_obs)
      {}
      int init_obs;
    };

  public:
    InitBaMSOO(const Options& options);
    virtual ~InitBaMSOO() = default;

  protected:
    void BuildGP() override;

  protected:
    void GetBOInitPoints();

  protected:
    int init_obs_;
    vecOfvec init_xs_;
    vectord init_ys_;
};

}
