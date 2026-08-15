#pragma once

#include "FastEvaluator/FastEvaluator.hpp"
#include "Individual.hpp"
#include "Rng/Rng.hpp"
#include "SmartPointer/SmartPointer.hpp"

class IndividualCrossover {
public:
  virtual void Cross(Individual *p1, SmartPointer<Individual> p2,
                     FastEvaluator &eval, SimpleRng &rng,
                     double cross_gene_p) = 0;
};
