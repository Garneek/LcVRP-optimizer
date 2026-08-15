#pragma once

#include "FastEvaluator/FastEvaluator.hpp"
#include "Individual.hpp"
#include "IndividualCrossover.hpp"
#include "Rng/Rng.hpp"
#include "SmartPointer/SmartPointer.hpp"

class UniformCross : public IndividualCrossover {
public:
  void Cross(Individual *p1, SmartPointer<Individual> p2, FastEvaluator &eval,
             SimpleRng &rng, double cross_gene_p) override {

    vector<int> new_sol1 = p1->MoveOutSolution();
    vector<int> new_sol2 = p2->MoveOutSolution();

    for (int i = 0; i < new_sol1.size(); i++)
      if (rng.GetDoubleUnif() < cross_gene_p)
        swap(new_sol1[i], new_sol2[i]);

    p1->MoveInSolution(std::move(new_sol1));
    p2->MoveInSolution(std::move(new_sol2));
  }
};
