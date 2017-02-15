/***********************************************************
* doo.h
* Implements DOO.
***********************************************************/
#pragma once
#include "cpplogo/optintf.h"
#include "cpplogo/node.h"

namespace cpplogo {

class DOO : public OptIntf {
/***********************************************************
* Options structure
***********************************************************/
  public:
    struct Options : public OptIntf::Options {
      Options(ObjectiveFn fn, int dim, int max_observations, 
              int num_children, double max_slope) :
        OptIntf::Options(fn, dim, num_children, max_observations),
        max_slope(max_slope)
      {}

      double max_slope;
    };

  public:
    DOO(const Options& opt);
    virtual ~DOO() = default;

  public:
    const Node& step_observed_node() const 
      { return *step_observed_node_; }

  protected:
    void BeginStep() override;
    void EndStep() override;
    void StepExpandNodes() override;
    size_t ChooseSplitDimension(const Node* node) const override;

  protected:
    double CalculateUpperBound(const Node& node);

  protected:
    double max_slope_;
    std::unique_ptr<Node> step_observed_node_; // Node that was observed 
                                               // in this step
};

}
