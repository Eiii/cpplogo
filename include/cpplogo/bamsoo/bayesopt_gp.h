#pragma once
#include "bayesopt.hpp"
#include "cpplogo/types.h"

namespace cpplogo {

class GP : public bayesopt::ContinuousModel
{
  public:
    GP(bopt_params params, const ObjectiveFn& fn, int dim) : 
      ContinuousModel(dim, params), fn_(fn) {}

    double evaluateSample(const boost::numeric::ublas::vector<double> &query)
    {
      return -fn_(query);
    }

    bool checkReachability(const boost::numeric::ublas::vector<double> &query)
    {
      (void)query;
      return true;
    }

    const ObjectiveFn& fn() const { return fn_; }

  protected:
    const ObjectiveFn& fn_;
};

}
