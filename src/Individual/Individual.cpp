#include "Individual.hpp"
#include <utility>
#include <vector>

unsigned long long Individual::Hash(const vector<int> &solution) {
  unsigned long long h = 1;
  for (int x : solution)
    // h = (x + 32h)  masked to 32 bits
    h = (x + (h << 5) - h) & 0xffffffff;
  return h;
}

Individual::Individual(int min_rand_int, int max_rand_int, int n,
                       SimpleRng &gen)
    : solution(n) {
  for (int i = 0; i < n; i++)
    solution[i] = gen.GetIntRange(0, max_rand_int);
  fitness = Option<double>::None;
}

Individual &Individual::operator=(Individual &other) {
  lock_guard<mutex> guard_other(other.ind_guard);
  lock_guard<mutex> guard_self(ind_guard);

  solution = other.solution;
  fitness = other.fitness;
  return *this;
}

double Individual::GetFitness(FastEvaluator &eval) {
  lock_guard<mutex> guard_self(ind_guard);
  if (fitness.isNone())
    fitness = eval.Evaluate(this->solution);

  return fitness.unpack();
}

void Individual::MoveInSolution(vector<int> &&new_sol) {
  lock_guard<mutex> guard_self(ind_guard);
  solution = std::move(new_sol);
  ResetFitness();
}

vector<int> Individual::MoveOutSolution() {
  lock_guard<mutex> guard_self(ind_guard);
  ResetFitness();

  return std::move(solution);
}

double Individual::GetProximity(SmartPointer<Individual> other) const {
  const vector<int> &other_sol = other->GetSolutionConst();
  if (other_sol.size() != solution.size())
    return -1;

  int matching = 0;
  for (int i = 0; i < solution.size(); i++)
    if (solution[i] != other_sol[i])
      matching++;

  return ((double)matching / (double)solution.size());
}

void Individual::InterRouteSwap(FastEvaluator &eval, SimpleRng &rng,
                                const OptimizerParams &params) {
  int c1 = rng.GetIntRange(0, solution.size() - 1);
  int c2 = rng.GetIntRange(0, solution.size() - 1);

  swap(solution[c1], solution[c2]);
  ResetFitness();
}

void Individual::InterRouteRelocate(FastEvaluator &eval, SimpleRng &rng,
                                    const OptimizerParams &params) {
  int client = rng.GetIntRange(0, solution.size() - 1);
  int new_group = rng.GetIntRange(0, eval.GetNumGroups() - 1);

  solution[client] = new_group;
  ResetFitness();
}

void Individual::InterRoute2Opt(FastEvaluator &eval, SimpleRng &rng,
                                const OptimizerParams &params) {
  int start_client1 = rng.GetIntRange(0, solution.size() - 1);
  int group1 = solution[start_client1];

  int start_client2;
  int group2;

  int i = 0;
  do {
    start_client2 = rng.GetIntRange(0, solution.size() - 1);
    group2 = solution[start_client2];
  } while (group1 == group2 && i++ < 100);

  for (int i = min(start_client1, start_client2); i < solution.size(); i++) {
    if (solution[i] == group1 && i >= start_client1)
      solution[i] = group2;
    else if (solution[i] == group2 && i >= start_client2)
      solution[i] = group1;
  }

  ResetFitness();
}

void Individual::InterRouteSearch(FastEvaluator &eval, SimpleRng &rng,
                                  const OptimizerParams &params) {
  int client = rng.GetIntRange(0, solution.size() - 1);

  int current_group = solution[client];

  double min_fitness = GetFitness(eval);
  int min_group = current_group;

  for (int group = 0; group < eval.GetNumGroups(); group++) {
    if (group != current_group) {
      ResetFitness();
      solution[client] = group;
      double f = GetFitness(eval);
      if (f < min_fitness) {
        min_fitness = f;
        min_group = group;
      }
    }
  }
  solution[client] = min_group;
  fitness = Option<double>::Some(min_fitness);
}

void Individual::LocalSearch(FastEvaluator &eval, SimpleRng &rng,
                             const OptimizerParams &params) {
  double strategy_tmp = rng.GetDoubleUnif();
  int strategy_index = 0;

  while (strategy_tmp > params.CUMULATIVE_LS_STRAT_PROPABLITIES[strategy_index])
    strategy_index++;

  switch (strategy_index) {
  case 0:
    InterRouteSwap(eval, rng, params);
    break;
  case 1:
    InterRouteRelocate(eval, rng, params);
    break;
  case 2:
    InterRoute2Opt(eval, rng, params);
    break;
  case 3:
    InterRouteSearch(eval, rng, params);
  }
}
