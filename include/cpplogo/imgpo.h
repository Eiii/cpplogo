/***********************************************************
* imgpo.h
* Implements the IMGPO algorithm.
***********************************************************/
#pragma once
#include "cpplogo/bamsoo.h"

namespace cpplogo {

class IMGPO : virtual public BaMSOO {
/***********************************************************
* Options structure
***********************************************************/
  public:
    struct Options : virtual public BaMSOO::Options {
      Options(ObjectiveFn fn, int dim, int max_observations, 
              int num_children, int max_depth) :
        SOO::Options(fn, dim, max_observations, num_children),
        BaMSOO::Options(fn, dim, max_observations, num_children),
        subtree_max_depth(max_depth)
      {}
      int subtree_max_depth;
    };

  public:
    IMGPO(const Options& options);
    virtual ~IMGPO() = default;

  protected:
    void BeginStep() override;
    void EndStep() override;
    size_t CalculateMaxDepth() const override;
    bool NodeShouldBeExpanded(Node* node) override;
    double BestSubtreeUCB(const Node* node, int depth);
    std::vector<Node> BuildSubtree(const Node* node, int depth);
    std::tuple<double, double> CalculateLCBUCB(const Node* node) override;

  protected:
    const int subtree_max_depth_;
    double current_subtree_depth_;
    double prev_best_;
};

}
