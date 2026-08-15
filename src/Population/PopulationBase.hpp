#pragma once

#include "Individual/Individual.hpp"
#include "Rng/Rng.hpp"
#include "SmartPointer/SmartPointer.hpp"
#include <mutex>

class PopulationBase {
private:
  mutex pop_guard;
  vector<SmartPointer<Individual>> pop;
  bool is_sorted;

protected:
  SmartPointer<Individual> GetIndividualAt(int idx);

public:
  PopulationBase() : pop(), is_sorted(false) {}
  ~PopulationBase() {}

  int GetPopSize();
  vector<SmartPointer<Individual>> &GetIndividuals();

  void InsertIndividual(SmartPointer<Individual> &ind);
  bool InsertIfAble(SmartPointer<Individual> &ind, int max_ind);

  void EnsureSorted(FastEvaluator &eval);

  double GetAverageFitness(FastEvaluator &eval);
  double GetMinFitness(FastEvaluator &eval);
  double GetMedianFitness(FastEvaluator &eval);
  void GeneralMutation(FastEvaluator &eval, SimpleRng &rng, double perc);

  virtual SmartPointer<Individual> Select(SimpleRng &rng,
                                          FastEvaluator &eval) = 0;
};
