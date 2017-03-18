#include "cpplogo/bamsoo.h"
#include "cpplogo/logging.h"
#include "cpplogo/common.h"

namespace cpplogo {

/***********************************************************
* default_params
* Helper function to initialize the bayesopt library's
* parameters.
***********************************************************/
namespace {
bopt_params default_params()
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

  //All irrelevant
  params.random_seed = 0; 
  params.n_init_samples = 0; 
  params.n_iterations = 0; 
  params.init_method = 3; 
  params.n_iter_relearn = 0; 
  params.force_jump = 0; 

  params.verbose_level = -1;

  return params;
} /* default_params() */
}

/***********************************************************
* BaMSOO constructor
***********************************************************/
BaMSOO::BaMSOO(const Options& options) :
  SOO(options), n_(0.5), gp_(default_params(), options.fn, options.dim)
{
} /* BaMSOO() */

/***********************************************************
* BeginStep
* BaMSOO might want to build a new GP if necessary at
* the beginning of each step.
***********************************************************/
void BaMSOO::BeginStep() 
{
  SOO::BeginStep();
  if (gp_.IsValid()) gp_.FitModel();
} /* BeginStep() */

/***********************************************************
* ObserveNodes
* When we need to observe values for some nodes, first
* build a GP using our past observations before making the
* observations.
***********************************************************/
void BaMSOO::ObserveNodes(std::vector<Node>* nodes)
{
  SOO::ObserveNodes(nodes);
} /* ObserveNodes() */

/***********************************************************
* ObserveNode
* Only 'actually' observe a node's value if the GP's upper
* bound for that node is greater than the best value we've
* seen so far. Otherwise, assign it the GP's lower bound so
* we effectively ignore it until we don't have any other
* option.
***********************************************************/
bool BaMSOO::ObserveNode(Node* node)
{
  bool sample_fn = false;
  double upper_bound, lower_bound;

  //If we don't have enough info to build a gp, just
  //observe the node.
  //If we already have a value for this node, then we don't
  //need to build the gp-- just pretend to observe it (which
  //does nothing)
  //Otherwise, calculate the bounds and figure out if we
  //should really observe it or not.
  if (!gp_.IsValid() || (node->has_value() && !node->is_fake_value())) {
    LOG(trace) << "Skipping GP generation";
    sample_fn = true;
  } else {
    //Calculate+use UCB/LCB
    std::tie(lower_bound, upper_bound) = CalculateLCBUCB(node);
    sample_fn = (upper_bound > BestNode()->value());
  }

  if (sample_fn) {
    bool observed = SOO::ObserveNode(node);
    if (observed) {
      const Node& obs_node = step_observed_nodes_.back();  
      gp_.AddSample(obs_node.Center(), obs_node.value());
    }
    return observed;
  } else {
    LOG(trace) << "Using lower bound -- setting to " << lower_bound;
    //If sample_fn is false, lower_bound must have been set.
    node->SetFakeValue(lower_bound);
    return false;
  }
} /* ObserveNode() */

/***********************************************************
* ExpandNode
***********************************************************/
std::vector<Node> BaMSOO::ExpandNode(Node* node)
{
  if (node->is_fake_value()) {
    SOO::ObserveNode(node);
  }
  return SOO::ExpandNode(node);
} /* ExpandNode() */

/***********************************************************
* CalculateLCBUCB
***********************************************************/
std::tuple<double, double> BaMSOO::CalculateLCBUCB(const Node* node)
{
  auto center = node->Center();
  bayesopt::ProbabilityDistribution* prediction = gp_.GetPrediction(center);
  double mean = prediction->getMean();
  double std = prediction->getStd();
  double mult = std::sqrt(
                  2.0 * 
                  std::log(
                    M_PI*M_PI*num_node_evals_*num_node_evals_/(6.0*n_)
                  )
                );
  double ucb = mean + mult*std;
  double lcb = mean - mult*std;
  LOG(trace) << "Mean: " << mean;
  LOG(trace) << "STD:  " << std;
  LOG(trace) << "Mult: " << mult;
  return std::make_tuple(lcb, ucb);
} /* CalculateLCBUCB() */

}
