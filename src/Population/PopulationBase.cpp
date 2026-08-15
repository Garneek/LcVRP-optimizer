#include "PopulationBase.hpp"

#include "Individual/SimpleMutator.hpp"
#include <algorithm>

SmartPointer<Individual> PopulationBase::GetIndividualAt(int idx) {
  lock_guard<mutex> guard(pop_guard);
  return pop[idx];
}

double PopulationBase::GetAverageFitness(FastEvaluator &eval) {
  lock_guard<mutex> guard(pop_guard);
  double sum = 0;
  for (SmartPointer<Individual> i : pop)
    sum += i->GetFitness(eval);
  return sum / ((double)pop.size());
}

double PopulationBase::GetMinFitness(FastEvaluator &eval) {
  EnsureSorted(eval);
  lock_guard<mutex> guard(pop_guard);
  return pop[0]->GetFitness(eval);
}

double PopulationBase::GetMedianFitness(FastEvaluator &eval) {
  EnsureSorted(eval);
  lock_guard<mutex> guard(pop_guard);
  return pop[pop.size() / 2]->GetFitness(eval);
}

void PopulationBase::GeneralMutation(FastEvaluator &eval, SimpleRng &rng,
                                     double perc) {
  for (SmartPointer<Individual> p : pop)
    if (rng.GetDoubleUnif() < perc)
      p->Mutate<SimpleMutator>(eval, rng, 1.0);
}

vector<SmartPointer<Individual>> &PopulationBase::GetIndividuals() {
  is_sorted = false;
  return pop;
}

void PopulationBase::InsertIndividual(SmartPointer<Individual> &ind) {
  lock_guard<mutex> guard(pop_guard);
  pop.push_back(ind);
  is_sorted = false;
}

int PopulationBase::GetPopSize() {
  lock_guard<mutex> guard(pop_guard);
  return pop.size();
}

void PopulationBase::EnsureSorted(FastEvaluator &eval) {
  lock_guard<mutex> guard(pop_guard);
  if (!is_sorted) {
    sort(pop.begin(), pop.end(),
         [&eval](SmartPointer<Individual> &a, SmartPointer<Individual> &b) {
           return a->GetFitness(eval) < b->GetFitness(eval);
         });
    is_sorted = true;
  }
}

bool PopulationBase::InsertIfAble(SmartPointer<Individual> &ind, int max_ind) {
  lock_guard<mutex> guard(pop_guard);

  if (pop.size() < max_ind) {
    pop.push_back(ind);
    is_sorted = false;
    return true;
  }
  return false;
}
