#include "MutationJob.hpp"
#include <cmath>
#include <cstdlib>

#include "Individual/OnePointCross.hpp"
#include "Individual/SimpleMutator.hpp"

void MutationJob::SetRemoveRepeats(bool new_r) {
  remove_repeats = new_r;

  lock_guard<mutex> guard(hashes_guard);
  hashes->clear();
}

void MutationJob::Message(ThreadJobMessage msg) {
  if (msg == ThreadJobMessage::MutationJobRemoveRepeatsOn)
    SetRemoveRepeats(true);
  if (msg == ThreadJobMessage::MutationJobRemoveRepeatsOff)
    SetRemoveRepeats(false);
}

void MutationJob::Anneal(SmartPointer<Individual> old,
                         SmartPointer<Individual> nw, double temperature) {
  double best = eval->GetBestFitness();
  double old_fit_scaled = old->GetFitness(*eval) / best;
  double new_fit_scaled = nw->GetFitness(*eval) / best;

  // new_fit_scaled *= (nw->GetProximity(old) / 100) + 0.995;

  if (old_fit_scaled < new_fit_scaled) {
    double accept_p =
        max(exp(-abs(old_fit_scaled - new_fit_scaled) / temperature), 0.001);
    if (rng.GetDoubleUnif() >= accept_p) {
      *nw = *old;
      return;
    }
  }

  *old = *nw;
}

void MutationJob::RunCycle(SmartPointer<PopulationBase> *pop_in,
                           SmartPointer<PopulationBase> *pop_out) {
  // Select two different individuals
  SmartPointer<Individual> ind1 = (*pop_in)->Select(rng, *eval);
  SmartPointer<Individual> ind2 = nullptr;
  do
    ind2 = (*pop_in)->Select(rng, *eval);
  while (ind2 == ind1);

  // Copy individuals to not modify original population, other threads are
  // still reading from it
  ind1 = new Individual(*ind1);
  ind2 = new Individual(*ind2);

  // Copy for annealing
  SmartPointer<Individual> tmp1 = new Individual(*ind1);
  SmartPointer<Individual> tmp2 = new Individual(*ind1);

  // Cross
  if (rng.GetDoubleUnif() < params.CROSS_P) {
    tmp1->Crossover<OnePointCross>(tmp2, *eval, rng, params.CROSS_GENE_P);

    Anneal(ind1, tmp1, params.TEMP);
    Anneal(ind2, tmp2, params.TEMP);
  }

  // Mutate
  if (rng.GetDoubleUnif() < params.MUT_PERC) {
    tmp1->Mutate<SimpleMutator>(*eval, rng, params.MUT_P);
    Anneal(ind1, tmp1, params.TEMP);
  }
  if (rng.GetDoubleUnif() < params.MUT_PERC) {
    tmp2->Mutate<SimpleMutator>(*eval, rng, params.MUT_P);
    Anneal(ind2, tmp2, params.TEMP);
  }

  for (int i = 0; i < params.N_ITERS_LS; i++) {
    tmp1->LocalSearch(*eval, rng, params);
    Anneal(ind1, tmp1, params.TEMP / (i + 1));

    tmp2->LocalSearch(*eval, rng, params);
    Anneal(ind2, tmp2, params.TEMP / (i + 1));
  }

  if (remove_repeats) {
    lock_guard<mutex> guard(hashes_guard);
    unsigned long long hash = ind1->HashSelf();
    if (hashes->find(hash) == hashes->end() || rng.GetDoubleUnif() < 0.01) {
      if ((*pop_out)->InsertIfAble(ind1, params.POP_SIZE))
        hashes->insert(hash);
    }

    hash = ind2->HashSelf();
    if (hashes->find(hash) == hashes->end() || rng.GetDoubleUnif() < 0.01) {
      if ((*pop_out)->InsertIfAble(ind2, params.POP_SIZE))
        hashes->insert(hash);
    }

  } else {
    (*pop_out)->InsertIfAble(ind1, params.POP_SIZE);
    (*pop_out)->InsertIfAble(ind2, params.POP_SIZE);
  }
}