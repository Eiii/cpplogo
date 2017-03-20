/***********************************************************
* randomsoo.h
* An extension to SOO in which the split dimension is 
* chosen randomly according to the same rule rather than
* deterministically.
***********************************************************/
#pragma once

#include "cpplogo/soo.h"

namespace cpplogo {

class RandomSOO : virtual public SOO {
  public:
    struct Options : virtual public SOO::Options {
      Options(ObjectiveFn fn, int dim, int max_observations, 
              int num_children, int seed) :
        SOO::Options(fn, dim, max_observations, num_children),
        seed(seed)
      {}
      int seed; // Seed with which to initialize the RNG
    };

  public:
    RandomSOO(const Options& opt);
    virtual ~RandomSOO() = default;

  protected:
    size_t ChooseSplitDimension(const Node* node) const override;

  protected:
    int seed_;
    mutable RandomEngine rng_;
    std::vector<size_t> order_;
};

}
