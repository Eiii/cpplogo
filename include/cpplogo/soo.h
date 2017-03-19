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
        OptIntf::Options(fn, dim, max_observations), num_children(num_children)
      {}
      int num_children; // Number of children each node splits into
    };

  public:
    SOO(const Options& opt);
    virtual ~SOO() = default;

  public:
    int num_expansions() const { return num_expansions_; }
    int num_node_evals() const { return num_node_evals_; }
    const std::vector<Node>& step_observed_nodes() const 
      { return step_observed_nodes_; }

  public:
    const Node* BestNode() const;

  protected:
    void BeginStep() override;
    void EndStep() override;
    size_t CalculateMaxDepth() const override;
    void StepExpandNodes() override;

  protected:
    virtual void ExpandBestAtDepth(size_t depth);
    virtual const Node* BestNodeAtDepth(size_t depth) const;
    virtual bool NodeShouldBeExpanded(Node* node);
    virtual size_t ChooseSplitDimension(const Node* node) const;
    virtual void ObserveNodes(std::vector<Node>* nodes);
    virtual bool ObserveNode(Node* node);
    virtual std::vector<Node> ExpandNode(Node* node);

  protected:
    Node* BestNodeAtDepth(size_t depth);
    void RemoveNode(Node* node);

  protected:
    // Variables from the options structure
    int num_children_;

    double vmax_;        // Best node value expanded in this step
    int num_expansions_; // Number of node expansions
    int num_node_evals_; // Number of node evaluations
    std::vector<std::vector<Node>> space_;  // All levels of nodes
    std::vector<Node> step_observed_nodes_; // All nodes that were observed in
                                            // this step
};

}
