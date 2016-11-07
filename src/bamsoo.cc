#include "cpplogo/bamsoo.h"
#include "cpplogo/logging.h"
#include "cpplogo/common.h"

namespace cpplogo {

/***********************************************************
* BaMSOO constructor
***********************************************************/
BaMSOO::BaMSOO(const Options& options) :
  SOO(options), gp_(nullptr) 
{
} /* BaMSOO() */

/***********************************************************
* ObserveNodes
* When we need to observe values for some nodes, first
* build a GP using our past observations before making the
* observations.
***********************************************************/
void BaMSOO::ObserveNodes(std::vector<Node>* nodes)
{
  BuildGP();
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
void BaMSOO::ObserveNode(Node* node)
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
  if (!gp_ || (node->has_value() && !node->is_fake_value())) {
    LOG(trace) << "Skipping GP generation";
    sample_fn = true;
  } else {
    //Calculate UCB/LCB
    auto center = node->Center();
    bayesopt::ProbabilityDistribution* prediction = gp_->getPrediction(center);
    double mean = prediction->getMean();
    double std = prediction->getStd();
    double n = 0.5; //TODO - ???
    double mult = std::sqrt(
                    2.0 * 
                    std::log(
                      M_PI*M_PI*num_node_evals_*num_node_evals_/(6.0*n)
                    )
                  );
    upper_bound = mean + mult*std;
    lower_bound = mean - mult*std;
    LOG(trace) << "Mean: " << mean;
    LOG(trace) << "STD:  " << std;
    LOG(trace) << "Mult: " << mult;

    const Node* best_node = BestNode();
    sample_fn = (upper_bound > best_node->value());
  }

  if (sample_fn) {
    SOO::ObserveNode(node);
  } else {
    LOG(trace) << "Using lower bound -- setting to " << lower_bound;
    node->SetFakeValue(lower_bound);
  }
} /* ObserveNode() */

/***********************************************************
* default_params
* Helper function to initialize the bayesopt library's
* parameters.
***********************************************************/
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

/***********************************************************
* ExpandNode
* TODO: Investigate if this is a good idea or not.
* When we expand a node, make sure it has a real value 
* before continuing! Otherwise we can just drill deeper
* and deeper into a part of the space we don't know very
* much about.
***********************************************************/
std::vector<Node> BaMSOO::ExpandNode(Node* node)
{
  // Disabled for now-- seems to degrade performance
  /*
  if (node->is_fake_value()) {
    SOO::ObserveNode(node);
  }
  */
  return SOO::ExpandNode(node);
} /* ExpandNode() */

/***********************************************************
* BuildGP
* Build a GP from the real observed samples so far.
***********************************************************/
void BaMSOO::BuildGP()
{
  vecOfvec xs; std::vector<double> std_ys;

  AddExistingPoints(&xs, &std_ys);

  vectord ys = std_to_boost(std_ys);

  InitGPPtr(xs, ys);
} /* BuildGP() */

/***********************************************************
* AddExistingPoints
* Add all observed real samples to the argument vectors.
***********************************************************/
void BaMSOO::AddExistingPoints(vecOfvec* xs, std::vector<double>* ys)
{
  //Init GP with all real observations
  for (const auto& node_list : space_) {
    for (const auto& node : node_list) {
      if (!node.is_fake_value()) {
        xs->push_back(node.Center());  
        ys->push_back(node.value());
      }
    }
  }
} /* AddExistingPoints() */

/***********************************************************
* InitGPPtr
* Use the provided sample data to initialize the object's
* GP for use during node observation.
***********************************************************/
void BaMSOO::InitGPPtr(const vecOfvec& xs, const vectord& ys)
{
  if (xs.size() < 2) {
    LOG(trace) << "Not enough data to build GP";
    gp_.reset(nullptr);
  } else {
    LOG(trace) << "Building GP with " << xs.size() << " samples.";
    gp_.reset(new GP(default_params(), fn_, dim_));
    gp_->initializeOptimizationWithPoints(xs, ys);
  }
} /* InitGPPtr() */

}
