#pragma once

class Individual;

#include "FastEvaluator/FastEvaluator.hpp"
#include "IndividualCrossover.hpp"
#include "IndividualMutator.hpp"
#include "OptimizerParams.hpp"
#include "Option/Option.hpp"
#include "Rng/Rng.hpp"
#include "SmartPointer/SmartPointer.hpp"
#include <mutex>
#include <type_traits>

using namespace std;

class Individual {
private:
  mutex ind_guard;
  vector<int> solution;

  // Option to reduce possibly costly lookup/evaluation operations in evaluator
  Option<double> fitness;

  void InterRouteSwap(FastEvaluator &eval, SimpleRng &rng,
                      const OptimizerParams &params);
  void InterRouteRelocate(FastEvaluator &eval, SimpleRng &rng,
                          const OptimizerParams &params);
  void InterRoute2Opt(FastEvaluator &eval, SimpleRng &rng,
                      const OptimizerParams &params);
  void InterRouteSearch(FastEvaluator &eval, SimpleRng &rng,
                        const OptimizerParams &params);

public:
  Individual() = delete;
  Individual(int n) : solution(n), fitness() {}
  Individual(const vector<int> &sol) : solution(sol), fitness() {}
  Individual(Individual &other) { *this = other; }
  Individual(Individual &&other) {
    fitness = other.fitness;
    solution = std::move(other.solution);
  }

  Individual(int min_rand_int, int max_rand_int, int n, SimpleRng &gen);
  ~Individual() {}

  Individual &operator=(Individual &other);

  static unsigned long long Hash(const vector<int> &solution);
  unsigned long long HashSelf() const { return Hash(solution); }

  vector<int> &GetSolution() { return solution; }
  const vector<int> &GetSolutionConst() const { return solution; }
  void MoveInSolution(vector<int> &&new_sol);
  vector<int> MoveOutSolution();

  double GetFitness(FastEvaluator &eval);
  void ResetFitness() { fitness = Option<double>::None; }

  double GetProximity(SmartPointer<Individual> other) const;

  template <typename T>
  void Mutate(FastEvaluator &eval, SimpleRng &rng, double mut_p) {
    static_assert(std::is_base_of<IndividualMutator, T>::value,
                  "T must be a child of IndividualMutator");

    T *mutator = new T();
    mutator->Mutate(this, eval, rng, mut_p);
    delete mutator;
  }

  template <typename T>
  void Crossover(SmartPointer<Individual> other, FastEvaluator &eval,
                 SimpleRng &rng, double cross_gene_p) {
    static_assert(std::is_base_of<IndividualCrossover, T>::value,
                  "T must be a child of IndividualMutator");

    T *crossover = new T();
    crossover->Cross(this, other, eval, rng, cross_gene_p);
    delete crossover;
  }

  void LocalSearch(FastEvaluator &eval, SimpleRng &rng,
                   const OptimizerParams &params);
};
