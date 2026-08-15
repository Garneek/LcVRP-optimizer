#include "Optimizer.hpp"
#include "FastEvaluator/FastEvaluator.hpp"
#include "Individual/Individual.hpp"
#include "OptimizerParams.hpp"
#include "Population/PopulationBase.hpp"
#include "Population/PopulationTournament.hpp"
#include "Rng/Rng.hpp"
#include "SmartPointer/SmartPointer.hpp"
#include "ThreadPool/ThreadPool.hpp"
#include <algorithm>
#include <iostream>
#include <limits>
#include <ostream>
#include <vector>

using namespace LcVRPContest;

Optimizer::Optimizer(FastEvaluator &evaluator, OptimizerParams &params)
    : params(params), rng(),
      current_best_fitness_(numeric_limits<double>::max()),
      fast_eval(evaluator.GetProblemData(), evaluator.GetNumGroups()),
      current_best_ind(nullptr), same_last_best_fitness(0), iteration(0),
      evaluator_(evaluator) {
  thread_pools.clear();
  thread_pools.reserve(params.NUM_POPULATIONS);
  for (int i = 0; i < params.NUM_POPULATIONS; i++) {
    thread_pools.push_back(ThreadPool(params, fast_eval));
    thread_pools[i].InitThreads();
  }
}

void Optimizer::Initialize() {
  current_best_.clear();
  current_best_.resize(fast_eval.GetSolutionSize());

  SmartPointer<Individual> ind =
      new Individual(fast_eval.GetLowerBound(), fast_eval.GetUpperBound(),
                     fast_eval.GetSolutionSize(), rng);

  current_best_fitness_ = ind->GetFitness(fast_eval);
  current_best_ = ind->GetSolution();
  current_best_ind = ind;
  populations = vector<SmartPointer<PopulationBase>>();
  populations.reserve(params.NUM_POPULATIONS);

  for (int pop_n = 0; pop_n < params.NUM_POPULATIONS; pop_n++) {
    SmartPointer<PopulationBase> new_pop(
        new PopulationTournament(params.TOURNAMENT_K));

    populations.push_back(new_pop);

    for (int i = 0; i < params.POP_SIZE; i++) {
      SmartPointer<Individual> tmp =
          new Individual(fast_eval.GetLowerBound(), fast_eval.GetUpperBound(),
                         fast_eval.GetSolutionSize(), rng);
      populations[pop_n]->InsertIndividual(tmp);
    }
  }
}
void Optimizer::TryGeneralMutation() {
  int locked_pops = 0;

  for (int pop_n = 0; pop_n < params.NUM_POPULATIONS; pop_n++) {
    populations[pop_n]->EnsureSorted(fast_eval);

    const vector<SmartPointer<Individual>> &pop =
        populations[pop_n]->GetIndividuals();
    populations[pop_n]->EnsureSorted(fast_eval);

    SmartPointer<Individual> best = pop[0];
    SmartPointer<Individual> far = pop[pop.size() - 2];

    if (far->GetFitness(fast_eval) == best->GetFitness(fast_eval))
      locked_pops++;
  }

  if (locked_pops == params.NUM_POPULATIONS) {
    for (int pop_n = 0; pop_n < params.NUM_POPULATIONS; pop_n++)
      populations[pop_n]->GeneralMutation(fast_eval, rng, params.GEN_MUT_PERC);

    params.TEMP = params.GEN_MUT_TEMPERATURE;
  }
}

void Optimizer::MutatePopulations() {
  vector<SmartPointer<PopulationBase>> new_pops;
  new_pops.reserve(populations.size());

  int i = 0;
  for (ThreadPool &pool : thread_pools) {
    new_pops.push_back(new PopulationTournament(params.TOURNAMENT_K));

    pool.StartMutation(populations[i], new_pops[i],
                       iteration % params.REMOVE_REPEATS_PERIOD == 0);
    i++;
  }

  i = 0;
  for (ThreadPool &pool : thread_pools) {
    pool.AwaitFinish();
    populations[i] = std::move(new_pops[i]);
    i++;
  }
}

void Optimizer::TryTransferBetweenPopulations() {
  if (iteration % params.TRANSFER_PERIOD == 0) {

    for (int pop_n = 0; pop_n < params.NUM_POPULATIONS; pop_n++)
      populations[pop_n]->EnsureSorted(fast_eval);

    SmartPointer<Individual> x = populations[0]->GetIndividuals()[0];
    for (int pop_n = 1; pop_n < params.NUM_POPULATIONS; pop_n++)
      populations[pop_n - 1]->GetIndividuals()[0] =
          populations[pop_n]->GetIndividuals()[0];

    populations[params.NUM_POPULATIONS - 1]->GetIndividuals()[0] = x;
  }
}

void Optimizer::UpdateBestIndividual() {
  for (int pop_n = 0; pop_n < params.NUM_POPULATIONS; pop_n++) {
    populations[pop_n]->EnsureSorted(fast_eval);
    SmartPointer<Individual> top_ind_from_pop =
        populations[pop_n]->GetIndividuals()[0];

    if (top_ind_from_pop->GetFitness(fast_eval) < current_best_fitness_) {
      current_best_ = top_ind_from_pop->GetSolution();
      current_best_fitness_ = top_ind_from_pop->GetFitness(fast_eval);
      current_best_ind = new Individual(*top_ind_from_pop);
    }
  }
}

void Optimizer::PrintIterationStatistics() {
  cout << iteration << ") " << current_best_fitness_;
  for (int pop_n = 0; pop_n < params.NUM_POPULATIONS; pop_n++)
    cout << "\t | " << populations[pop_n]->GetMinFitness(fast_eval) << ",\t"
         << populations[pop_n]->GetAverageFitness(fast_eval) << ",\t"
         << populations[pop_n]->GetMedianFitness(fast_eval);
  cout << "\t| " << current_best_fitness_ - last_best_fitness << "\t| "
       << params.TEMP;
  cout << endl;
}

void Optimizer::CoolDown() {
  params.TEMP *= params.TEMP_MULT;
  params.TEMP = max(params.TEMP, 1e-16);
}

void Optimizer::RunIteration() {
  iteration++;

  TryGeneralMutation();
  MutatePopulations();
  UpdateBestIndividual();
  TryTransferBetweenPopulations();
  if (iteration % 50 == 0 && !params.QUIET)
    PrintIterationStatistics();
  CoolDown();

  if (last_best_fitness == current_best_fitness_)
    same_last_best_fitness++;
  else
    same_last_best_fitness = 0;
  last_best_fitness = current_best_fitness_;
}