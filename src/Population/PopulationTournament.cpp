#include "PopulationTournament.hpp"
#include <algorithm>
#include <limits>

SmartPointer<Individual> PopulationTournament::Select(SimpleRng &rng,
                                                      FastEvaluator &eval) {
  PopulationBase::EnsureSorted(eval);
  int pop_size = PopulationBase::GetPopSize();

  int max_rand_int = pop_size - 1;
  int min_idx = numeric_limits<int>::max();

  // Since the list of individuals is sorted we select a random individual with
  // lowest idx, which is equalivient to later searching the indices for the
  // smallest fitness
  for (int i = 0; i < k; i++) {
    int idx = rng.GetIntRange(0, max_rand_int);

    // if (rng.get_double_unif() < p || min_idx == numeric_limits<int>::max())
    min_idx = min(min_idx, idx);
  }

  return PopulationBase::GetIndividualAt(min_idx);
}