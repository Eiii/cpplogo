#pragma once
#include "bayesopt/bayesopt.hpp"
#include "cpplogo/types.h"
#include "posteriormodel.hpp"
#include "randgen.hpp"

#include <memory>

namespace cpplogo {

using bayesopt::vecOfvec;

class GP
{
  public:
    GP(bopt_params params, const ObjectiveFn& fn, int dim) : 
    fn_(fn), model_(nullptr), rng_(), samples_(0)
    {
      FILELog::ReportingLevel() = logERROR;
      rng_.seed(params.random_seed);
      model_.reset(bayesopt::PosteriorModel::create(dim, params, rng_));
    }

    bayesopt::ProbabilityDistribution* GetPrediction(const vectord& x) { 
      assert(IsValid());
      return model_->getPrediction(x); 
    }
    void FitModel() { model_->updateHyperParameters(); model_->fitSurrogateModel(); }
    void AddSample(const vectord& x, double y) { 
      if (samples_ == 0) {
        model_->setSample(x, y);
      } else {
        model_->addSample(x, y); 
      }
      samples_++; 
      model_->updateSurrogateModel(); 
    }
    bool IsValid() { return samples_ >= 2; }
    const ObjectiveFn& fn() const { return fn_; }

  protected:
    const ObjectiveFn& fn_;
    std::unique_ptr<bayesopt::PosteriorModel> model_;
    randEngine rng_;
    int samples_;
};

}
