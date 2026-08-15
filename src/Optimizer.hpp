#pragma once

#include "FastEvaluator/FastEvaluator.hpp"
#include "Population/PopulationBase.hpp"
#include "SmartPointer/SmartPointer.hpp"
#include "ThreadPool/ThreadPool.hpp"

#include "OptimizerParams.hpp"

#include <vector>

using namespace std;

namespace LcVRPContest {

class Optimizer {
public:
  Optimizer(FastEvaluator &evaluator, OptimizerParams &params);
  void Initialize();
  void RunIteration();
  vector<int> *GetCurrentBest() { return &current_best_; }
  double GetCurrentBestFitness() const { return current_best_fitness_; }

private:
  FastEvaluator &evaluator_;
  vector<int> current_best_;

  OptimizerParams &params;

  double current_best_fitness_;
  int same_last_best_fitness;
  double last_best_fitness;

  int iteration;
  SmartPointer<Individual> current_best_ind;

  FastEvaluator fast_eval;
  vector<SmartPointer<PopulationBase>> populations;

  vector<ThreadPool> thread_pools;

  SimpleRng rng;

  void TryGeneralMutation();
  void MutatePopulations();
  void UpdateBestIndividual();
  void TryTransferBetweenPopulations();
  void PrintIterationStatistics();
  void CoolDown();
};
} // namespace LcVRPContest