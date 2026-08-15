#pragma once

#include "FastEvaluator/FastEvaluator.hpp"
#include "Individual.hpp"
#include "Rng/Rng.hpp"
#include "SmartPointer/SmartPointer.hpp"

class IndividualMutator {
public:
  virtual void Mutate(Individual *ind, FastEvaluator &eval, SimpleRng &rng,
                      double mut_p) = 0;
};
