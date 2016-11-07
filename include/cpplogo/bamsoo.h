/***********************************************************
* bamsoo.h
* Implements the BaMSOO algorithm.
***********************************************************/
#pragma once
#include "cpplogo/soo.h"
#include "cpplogo/bamsoo/bayesopt_gp.h"

namespace cpplogo {

class BaMSOO : virtual public SOO {
/***********************************************************
* Options structure
***********************************************************/
  public:
    struct Options : virtual public SOO::Options {
      Options(ObjectiveFn fn, int dim, int max_observations, 
              int num_children) :
        SOO::Options(fn, dim, max_observations, num_children)
      {}
    };

  public:
    BaMSOO(const Options& options);
    virtual ~BaMSOO() = default;

  protected:
    virtual void ObserveNodes(std::vector<Node>* nodes);
    virtual void ObserveNode(Node* node);
    virtual void BuildGP();

  protected:
    void AddExistingPoints(vecOfvec* xs, std::vector<double>* ys);
    void InitGPPtr(const vecOfvec& xs, const vectord& ys);

  protected:
    std::unique_ptr<GP> gp_; // Pointer to GP used to estimate node bounds
};

}
