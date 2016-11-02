#include <iostream>

#include "cpplogo/logging.h"
#include "cpplogo/node.h"
#include "cpplogo/soo.h"
#include "cpplogo/logo.h"

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
template <typename Alg, typename... OptArgs>
void evaluate(const Function& fn, double epsilon, OptArgs... args) 
{
  typename Alg::Options opt(fn.fn, fn.dim, 8000, 3, args...);
  Alg alg(opt);

  double best = alg.BestNode()->value();
  while (obs_error(best, fn) > epsilon) {
    alg.Step();  
    best = alg.BestNode()->value();
  }
  LOG(output) << "Number of function evaluations: " << alg.num_observations();
  LOG(output) << "Error: " << obs_error(alg.BestNode()->value(), fn);
  LOG(output) << "Best: " << alg.BestNode()->Center();
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

int main() {
  //Replace `output` with `trace` for more detailed log output.
  init_logging(output);

  LOG(output) << "SOO, rosenbrock2";
  evaluate<SOO>(rosenbrock2_fn, 1e-4);
  LOG(output) << "LOGO, rosenbrock2";
  evaluate<LOGO, std::vector<int>>(rosenbrock2_fn, 1e-4, {3, 4, 5, 6, 8, 30});
  LOG(output) << "SOO, rosenbrock10";
  evaluate<SOO>(rosenbrock10_fn, 1e-4);
  LOG(output) << "LOGO, rosenbrock10";
  evaluate<LOGO, std::vector<int>>(rosenbrock10_fn, 1e-4, {3, 4, 5, 6, 8, 30});

  return 0;
}
