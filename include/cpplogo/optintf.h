/***********************************************************
* optintf.h - Optimization Interface
* Defines the basic interface and high-level functionality
* for any SOO-like optimization algorithms that will be 
* implemented.
***********************************************************/
#pragma once
#include "cpplogo/types.h"

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
      Options(ObjectiveFn fn, int dim, int max_observations) :
        fn(fn), dim(dim), max_observations(max_observations) {};
      ObjectiveFn fn;       // Objective function to optimize
      int dim;              // Dimensionality of objective
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

  public:
    int num_observations() const { return num_observations_; }

  protected:
    virtual void BeginStep() = 0;
    virtual void EndStep() = 0;
    virtual int CalculateMaxDepth() const = 0;
    virtual void ExpandBestAtDepth(int depth) = 0;

  protected:
    // Variables from the options structure
    ObjectiveFn fn_;
    int dim_;
    int max_observations_;

    int num_observations_;  // Number of function observations so far
};

}
