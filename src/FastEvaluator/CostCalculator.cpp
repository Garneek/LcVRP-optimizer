#include "CostCalculator.hpp"
#include <vector>

using namespace FastEval;

double CostCalculator::GetRouteCost(const FastEvaluator &eval,
                                    const vector<int> &route) {
  CostCalculator calculator(eval, route);
  return calculator.get_cost();
}

CostCalculator::CostCalculator(const FastEvaluator &eval,
                               const vector<int> &route)
    : route(route), eval(eval) {

  cap = eval.GetProblemData().GetCapacity();
  dep_idx = eval.GetProblemData().GetDepot() - 1;
  max_dist = eval.GetProblemData().HasDistanceConstraint()
                 ? eval.GetProblemData().GetDistance()
                 : Option<double>::None;

  last_pos = dep_idx;
  cur_space_left = cap;

  cur_dist = 0.0;
  cost = 0.0;
}

inline bool CostCalculator::return_to_dep() {
  double d = eval.GetDistance(last_pos, dep_idx);
  if (d < 0.0)
    return false;

  last_pos = dep_idx;

  cost += cur_dist + d;

  cur_space_left = cap;
  cur_dist = 0.0;

  return true;
}

inline double CostCalculator::get_cost() {
  const vector<int> &demands = eval.GetProblemData().GetDemands();
  for (int idx : route) {
    idx -= 1;
    int demand = demands[idx];
    if (cur_space_left - demand < 0)
      // reroute to depot
      if (!return_to_dep())
        return WRONG_VAL;

    double d = eval.GetDistance(last_pos, idx);
    if (d < 0)
      return WRONG_VAL;

    if (max_dist.isSome()) {
      double d_to_dep = eval.GetDistance(idx, dep_idx);
      if (d_to_dep < 0)
        return WRONG_VAL;

      if (cur_dist + d + d_to_dep > max_dist.unpack()) {
        // reroute to depot
        if (!return_to_dep())
          return WRONG_VAL;

        d = eval.GetDistance(last_pos, idx);
        if (d < 0)
          return WRONG_VAL;
      }
    }

    cur_dist += d;
    cur_space_left -= demand;
    last_pos = idx;
  }
  if (!return_to_dep())
    return WRONG_VAL;
  return cost;
}
