#include "FastEvaluator.hpp"
#include "CostCalculator.hpp"
#include "Individual/Individual.hpp"
#include <limits>
#include <mutex>

void FastEvaluator::PrecalculateDistances() {
  int n_nodes = problem_data.GetDimension();
  distances.resize(n_nodes);

  for (int i = 0; i < n_nodes; i++) {
    distances[i].resize(i + 1);
    for (int j = 0; j < i + 1; j++)
      distances[i][j] = problem_data.CalculateDistance(i, j);
  }
}

bool FastEvaluator::ValidateProblemData() {
  int dep_idx = problem_data.GetDepot() - 1;
  const vector<int> &demands = problem_data.GetDemands();
  int cap = problem_data.GetCapacity();

  for (int customer_id = 2; customer_id <= problem_data.GetDimension();
       customer_id++)
    if (demands[customer_id - 1] > cap)
      return false;

  if (problem_data.HasDistanceConstraint()) {
    double max_distance = problem_data.GetDistance();
    for (int customer_id = 2; customer_id <= problem_data.GetDimension();
         customer_id++) {
      int idx = customer_id - 1;
      double dist = distances[dep_idx][idx];
      if (dist < 0.0 || dist * 2.0 > max_distance)
        return false;
    }
  }
  return true;
}

FastEvaluator::FastEvaluator(const ProblemData &problem_data, int num_groups)
    : problem_data(problem_data), num_groups(num_groups),
      num_customers(problem_data.GetNumCustomers()) {
  PrecalculateDistances();
  valid_data = ValidateProblemData();
  best_fitness = numeric_limits<double>::max();
}

double FastEvaluator::Evaluate(const int *solution) {
  if (solution == nullptr)
    return WRONG_VAL;

  vector<int> grouping(solution, solution + num_customers);
  return Evaluate(grouping);
}

double FastEvaluator::Evaluate(const vector<int> *solution) {
  if (solution == nullptr)
    return WRONG_VAL;
  return Evaluate(*solution);
}

double FastEvaluator::Evaluate(const vector<int> &solution) {
  if (!valid_data)
    return WRONG_VAL;

  if (solution.size() != static_cast<size_t>(GetSolutionSize()))
    return WRONG_VAL;

  for (int g : solution)
    if (g < GetLowerBound() || g > GetUpperBound())
      return WRONG_VAL;

  vector<vector<int>> routes;
  BuildRoutes(solution, routes);

  double sum = 0;
  for (const vector<int> &route : routes) {
    double cost = CalculateRouteCost(route);
    if (cost == WRONG_VAL)
      return cost;
    else
      sum += cost;
  }

  {
    lock_guard<mutex> guard(best_fitness_guard);
    if (sum < best_fitness)
      best_fitness = sum;
  }

  return sum;
}

double FastEvaluator::GetBestFitness() {
  lock_guard<mutex> guard(best_fitness_guard);
  return best_fitness;
}

void FastEvaluator::BuildRoutes(const vector<int> &grouping,
                                vector<vector<int>> &routes_target) {
  vector<vector<int>> routes(num_groups, vector<int>());
  for (int i = 0; i < num_groups; i++)
    routes[i].reserve(100);

  const vector<int> &permutation = problem_data.GetPermutation();

  for (size_t i = 0; i < permutation.size(); ++i) {
    int customer_id = permutation[i];
    int customer_index = customer_id - 2;

    if (customer_index >= 0 && customer_index < num_customers) {
      int group = grouping[customer_index];
      routes[group].push_back(customer_id);
    }
  }
  routes_target = std::move(routes);
}

double FastEvaluator::CalculateRouteCost(const vector<int> &route) {
  double v = FastEval::CostCalculator::GetRouteCost(*this, route);
  return v;
}
