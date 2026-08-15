#pragma once

#include "FastEvaluator/FastEvaluator.hpp"
#include "Individual.hpp"
#include "IndividualMutator.hpp"
#include "Rng/Rng.hpp"
#include "SmartPointer/SmartPointer.hpp"
#include <vector>

class SimpleMutator : public IndividualMutator {
public:
  void Mutate(Individual *ind, FastEvaluator &eval, SimpleRng &rng,
              double mut_p) override {
    ind->ResetFitness();

    vector<int> &sol = ind->GetSolution();
    for (int i = 0; i < sol.size(); i++) {
      if (rng.GetDoubleUnif() < mut_p)
        sol[i] = rng.GetIntRange(eval.GetLowerBound(), eval.GetUpperBound());
    }
  }
};
