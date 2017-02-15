/***********************************************************
* optintf.h - Optimization Interface
* Defines the basic interface and high-level functionality
* for any SOO-like optimization algorithms that will be 
* implemented.
***********************************************************/
#pragma once
#include "cpplogo/types.h"
#include "cpplogo/node.h"

namespace cpplogo {

class OptIntf {
/***********************************************************
* Options structure
* A collection of information required for the optimization.
* These variables are just collected in a structure to make
* the constructor easier to deal with.
***********************************************************/
  public:
    struct Options {
      Options(ObjectiveFn fn, int dim, int num_children, int max_observations) :
        fn(fn), dim(dim), num_children(num_children), 
        max_observations(max_observations) {};
      ObjectiveFn fn;       // Objective function to optimize
      int dim;              // Dimensionality of objective
      int num_children;     // Number of children each node split produces
      int max_observations; // Maximum number of function observations before
                            // stopping
    };

  public:
    OptIntf(const Options& opt);
    virtual ~OptIntf() = default;

  public:
    void Optimize();
    void Step();
    bool IsFinished() const;
    const Node* BestNode() const;

  public:
    int num_observations() const { return num_observations_; }

  protected:
    void RemoveNode(Node* node);

  protected:
    virtual std::vector<Node> ExpandNode(Node* node);
    virtual bool ObserveNode(Node* node);
    virtual void ObserveNodes(std::vector<Node>* nodes);
    virtual void AddNode(const Node& node);
    virtual void AddNodes(const std::vector<Node>& nodes);
    virtual void BeginStep() = 0;
    virtual void EndStep() = 0;
    virtual void StepExpandNodes() = 0;
    virtual size_t ChooseSplitDimension(const Node* node) const = 0;

  protected:
    // Variables from the options structure
    ObjectiveFn fn_;
    int dim_;
    int num_children_;
    int max_observations_;

    int num_observations_;  // Number of function observations so far
    std::vector<std::vector<Node>> space_;  // All levels of nodes
};

}
