#pragma once

#include "FastEvaluator.hpp"
#include "Option/Option.hpp"

#include <vector>

using namespace std;

namespace FastEval {
class CostCalculator {
private:
  const double WRONG_VAL = -1.0;
  const FastEvaluator &eval;

  int cap;
  int dep_idx;
  Option<double> max_dist;
  const vector<int> &route;

  int last_pos;
  int cur_space_left;

  double cur_dist;
  double cost;

  CostCalculator(const FastEvaluator &eval, const vector<int> &route);
  bool return_to_dep();
  double get_cost();

public:
  CostCalculator() = delete;
  static double GetRouteCost(const FastEvaluator &eval,
                             const vector<int> &route);
};

} // namespace FastEval