/***********************************************************
* randomimgpo.h
* Implements IMGPO with random split dimension selection.
***********************************************************/
#pragma once
#include "cpplogo/imgpo.h"
#include "cpplogo/randomsoo.h"
#include "cpplogo/logging.h"

namespace cpplogo {

struct cmp_node {
  bool operator()(const Node& n1, const Node& n2) const {
    if (n1.depth() < n2.depth()) return true;
    size_t dim = n1.sizes().size();
    for (size_t i = 0; i < dim; i++) {
      if (n1.sizes()[i] < n2.sizes()[i]) return true;
      if (n1.edges()[i] < n2.edges()[i]) return true;
    }
    LOG(output) << "MATCH: " << n1 << " / " << n2;
    return false;
  } 
};

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
      SOO(opt), BaMSOO(opt), RandomSOO(opt), IMGPO(opt),
      pre_splits_() {}
    virtual ~RandomIMGPO() = default;

  protected:
    size_t ChooseSplitDimension(const Node* node) const override;

  protected:
    mutable std::vector<std::pair<Node, size_t>> pre_splits_;
};

}
