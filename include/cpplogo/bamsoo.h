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
    void BeginStep() override;
    void ObserveNodes(std::vector<Node>* nodes) override;
    bool ObserveNode(Node* node) override;
    std::vector<Node> ExpandNode(Node* node) override;
    virtual std::tuple<double, double> CalculateLCBUCB(const Node* node);

  protected:
    double n_;
    GP gp_;
};

}
