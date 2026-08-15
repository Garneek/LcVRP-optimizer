#pragma once

#include "Individual/Individual.hpp"
#include "PopulationBase.hpp"
#include "SmartPointer/SmartPointer.hpp"

class PopulationTournament : public PopulationBase {
  int k;

public:
  PopulationTournament() : PopulationBase() {}
  PopulationTournament(int k) : PopulationBase() { this->k = k; }

  SmartPointer<Individual> Select(SimpleRng &rng, FastEvaluator &eval) override;
};
