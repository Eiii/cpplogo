#include "cpplogo/initbamsoo.h"
#include "cpplogo/logging.h"
#include "cpplogo/common.h"

namespace cpplogo {

/***********************************************************
* InitBaMSOO constructor
***********************************************************/
InitBaMSOO::InitBaMSOO(const Options& options) :
  SOO(options), RandomBaMSOO(options), 
  init_obs_(options.init_obs), init_xs_(), init_ys_() 
{
  GetBOInitPoints();
} /* InitBaMSOO() */

/***********************************************************
* InitialSamples
* Return the samples that the GP is initialized with.
***********************************************************/
std::vector<std::pair<vectord, double>> InitBaMSOO::InitialSamples() const
{
  std::vector<std::pair<vectord, double>> samples;
  assert(init_xs_.size() == init_ys_.size());
  for (size_t i = 0; i < init_xs_.size(); i++) {
    samples.emplace_back(init_xs_[i], init_ys_[i]); 
  }
  return samples;
} /* InitialSamples() */

/***********************************************************
* default_params
* Helper function to initialize the bayesopt library's
* parameters.
***********************************************************/
namespace {
bopt_params default_params(int seed, int init_obs)
{
  bopt_params params = initialize_parameters_to_default();

  set_criteria(&params, "cEI");
  params.n_crit_params = 0;

  set_kernel(&params, "kSEISO");
  params.kernel.hp_mean[0] = 1.0;
  params.kernel.hp_std[0] = 1.0;
  params.kernel.n_hp = 1;

  set_mean(&params,"mConst"); //Mean 0, std 0
  params.mean.coef_mean[0] = 0.0;
  params.mean.coef_std[0] = 1.0;
  params.mean.n_coef = 1;

  set_surrogate(&params, "sGaussianProcessML");
  params.noise = 1e-12;

  params.sc_type = SC_MTL; //Kernel parameter learning type
  params.l_type = L_EMPIRICAL;
  params.l_all = 1;

  params.random_seed = seed; 
  params.n_init_samples = init_obs; 

  //All irrelevant
  params.n_iterations = 0; 
  params.init_method = 3; 
  params.n_iter_relearn = 0; 
  params.force_jump = 0; 

  params.verbose_level = -1;

  return params;
} /* default_params() */
}

/***********************************************************
* GetBOInitPoints
* Create a bayesopt instance and call a secret hack of
* a function that returns the same initial points a bayesopt
* instance with the specified seed would start with.
* This is just so that comparisons between InitBaMSOO and
* BO are as fair as possible.
***********************************************************/
void InitBaMSOO::GetBOInitPoints()
{
  GP init_gp(default_params(seed_, init_obs_), fn_, dim_); 
  std::tie(init_xs_, init_ys_) = init_gp.getInitialPoints();
} /* GetBOInitPoints() */

/***********************************************************
* BuildGP
* When we build the GP, make sure we include the initial
* points too!
***********************************************************/
void InitBaMSOO::BuildGP()
{
  vecOfvec xs; std::vector<double> std_ys;

  AddExistingPoints(&xs, &std_ys);

  //Append the initial points we would have gotten from the GP
  xs.insert(xs.end(), init_xs_.begin(), init_xs_.end());
  std_ys.insert(std_ys.end(), init_ys_.begin(), init_ys_.end());

  vectord ys = std_to_boost(std_ys);

  InitGPPtr(xs, ys);
} /* BuildGP() */

}
