/***********************************************************
* soo.h
* Implements SOO.
***********************************************************/
#pragma once
#include "cpplogo/optintf.h"
#include "cpplogo/node.h"

namespace cpplogo {

class SOO : public OptIntf {
/***********************************************************
* Options structure
***********************************************************/
  public:
    struct Options : public OptIntf::Options {
      Options(ObjectiveFn fn, int dim, int max_observations, 
              int num_children) :
        OptIntf::Options(fn, dim, num_children, max_observations)
      {}
    };

  public:
    SOO(const Options& opt);
    virtual ~SOO() = default;

  public:
    int num_expansions() const { return num_expansions_; }
    int num_node_evals() const { return num_node_evals_; }
    const std::vector<Node>& step_observed_nodes() const 
      { return step_observed_nodes_; }

  protected:
    void BeginStep() override;
    void EndStep() override;
    void StepExpandNodes() override;
    size_t ChooseSplitDimension(const Node* node) const override;
    bool ObserveNode(Node* node) override;

  protected:
    virtual size_t CalculateMaxDepth() const;
    virtual void ExpandBestAtDepth(size_t depth);
    virtual const Node* BestNodeAtDepth(size_t depth) const;

  protected:
    Node* BestNodeAtDepth(size_t depth);

  protected:
    double vmax_;        // Best node value expanded in this step
    int num_expansions_; // Number of node expansions
    int num_node_evals_; // Number of node evaluations
    std::vector<Node> step_observed_nodes_; // All nodes that were observed in
                                            // this step
};

}
