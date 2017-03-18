#include <iostream>

#include "cpplogo/logging.h"
#include "cpplogo/node.h"
#include "cpplogo/soo.h"
#include "cpplogo/randomsoo.h"
#include "cpplogo/logo.h"
#include "cpplogo/randomlogo.h"
#include "cpplogo/bamsoo.h"
#include "cpplogo/randombamsoo.h"
#include "cpplogo/bamlogo.h"
#include "cpplogo/randombamlogo.h"

using std::vector;
using namespace cpplogo;

struct Function {
  cpplogo::ObjectiveFn fn;
  double max;
  int dim;
};

//Calculate the error of an function observation
double obs_error(double best, const Function& fn) 
{
  double diff = fn.max - best;
  if (fn.max == 0.0f) {
    return fabs(diff);
  } else {
    return fabs(diff / fn.max);
  }
}

//Evaluate a given SOO-like algorithm on a given function until
//it reaches error epsilon
constexpr int c_max_obs = 500;
constexpr int c_num_children = 3;

template <typename Alg, typename... OptArgs>
void evaluate(const Function& fn, double epsilon, OptArgs... args) 
{
  typename Alg::Options opt(fn.fn, fn.dim, c_max_obs, c_num_children, args...);
  Alg alg(opt);

  double best = alg.BestNode()->value();
  while (obs_error(best, fn) > epsilon && !alg.IsFinished()) {
    alg.Step();  
    best = alg.BestNode()->value();
  }
  LOG(output) << "Number of function evaluations: " << alg.num_observations();
  LOG(output) << "Error: " << obs_error(alg.BestNode()->value(), fn);
  LOG(output) << "Best: " << alg.BestNode()->Center();
}

template <typename Alg, typename... OptArgs>
void evaluate_many(const Function& fn, double epsilon, int count, OptArgs... args) 
{
  vector<int> obs;
  for (int seed = 0; seed < count; seed++) {
    typename Alg::Options opt(fn.fn, fn.dim, c_max_obs, c_num_children, 
                              seed, args...);
    Alg alg(opt);

    double best = alg.BestNode()->value();
    while (obs_error(best, fn) > epsilon && !alg.IsFinished()) {
      alg.Step();  
      best = alg.BestNode()->value();
    }
    obs.push_back(alg.num_observations());
  }
  LOG(output) << "# evaluations over " << count << " runs: ";
  LOG(output) << "  Min: " << *std::min_element(obs.begin(), obs.end());
  LOG(output) << "  Max: " << *std::max_element(obs.begin(), obs.end());
  double avg = std::accumulate(obs.begin(), obs.end(), 0) 
               / static_cast<double>(obs.size());
  LOG(output) << "  Avg: " << avg;
}

//Objective functions
double rosenbrock(const vectord& input) {
  double min = -5.0; double max = 10.0;
  vectord it(input);
  for (auto& x : it) x = min + x * (max - min);
  double sum = 0.0;
  for (size_t i = 0; i < it.size()-1; i++) {
    sum += 100.0 * pow(it(i+1) - it(i) * it(i), 2.0) 
           + pow(1.0 - it(i), 2.0);
  }
  return -sum;
}

Function rosenbrock2_fn {
  .fn = rosenbrock,
  .max = 0.0,
  .dim = 2,
};

Function rosenbrock10_fn {
  .fn = rosenbrock,
  .max = 0.0,
  .dim = 10,
};

void test_all_on_fn(const Function& fn) {
  vector<int> logo_w = {3, 4, 5, 6, 8, 30};

  LOG(output) << "-- SOO:";
  evaluate<SOO>(fn, 1e-4);
  LOG(output) << "-- LOGO:";
  evaluate<LOGO>(fn, 1e-4, logo_w);
  LOG(output) << "-- BaMSOO:";
  evaluate<BaMSOO>(fn, 1e-4);
  LOG(output) << "-- BaMLOGO:";
  evaluate<BaMLOGO>(fn, 1e-4, logo_w);

  //Randomized algorithms
  LOG(output) << "-- RandomSOO:";
  evaluate_many<RandomSOO>(fn, 1e-4, 100);
  LOG(output) << "-- RandomLOGO:";
  evaluate_many<RandomLOGO>(fn, 1e-4, 100, logo_w);
  LOG(output) << "-- RandomBaMSOO:";
  evaluate_many<RandomBaMSOO>(fn, 1e-4, 10);
  LOG(output) << "-- RandomBaMLOGO:";
  evaluate_many<RandomBaMLOGO>(fn, 1e-4, 10, logo_w);
}

int main() {
  //Replace `output` with `trace` for more detailed log output.
  init_logging(output);

  LOG(output) << "-- BaMSOO:";
  evaluate<BaMSOO>(rosenbrock2_fn, 1e-2);

  return 0;
}
