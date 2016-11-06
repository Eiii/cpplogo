/***********************************************************
* logo.h
* Implements the LOGO algorithm.
***********************************************************/
#pragma once
#include "cpplogo/soo.h"

namespace cpplogo {

class LOGO : virtual public SOO {
/***********************************************************
* Options structure
***********************************************************/
  public:
    struct Options : virtual public SOO::Options {
      Options(ObjectiveFn fn, int dim, int max_observations, 
              int num_children, std::vector<int> w_schedule) :
        SOO::Options(fn, dim, max_observations, num_children),
        w_schedule(w_schedule)
      {}
      std::vector<int> w_schedule; // List of W values to move through
    };

  public:
    LOGO(const Options& opt);
    virtual ~LOGO() = default;

  protected:
    void EndStep() override;
    int CalculateMaxDepth() const override;
    Node* BestNodeAtDepth(int depthset_id) override;

  protected:
    // Variables from the options structure
    std::vector<int> w_schedule_;

    int depthset_width_;   // Current width of depth levels to consider (w)
    double last_best_obs_; // Best value observed in the previous step
};

}
