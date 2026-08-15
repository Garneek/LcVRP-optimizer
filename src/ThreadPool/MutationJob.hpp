#pragma once

#include "OptimizerParams.hpp"
#include "Rng/Rng.hpp"
#include "SmartPointer/SmartPointer.hpp"
#include "ThreadJob.hpp"

#include <atomic>
#include <mutex>
#include <unordered_set>

class MutationJob : public ThreadJob {
  // Hashes used for removing repeats
  mutex hashes_guard;
  SmartPointer<std::unordered_set<unsigned long long>> hashes;

  // Triggered every some iterations because of compute requirements,
  // regenerates new individuals until there are no repeats
  std::atomic_bool remove_repeats;

  // Eval is thread safe evaluator + lookup table optimization
  // This is a temporary pointer, the value is owned by Optimizer object, thus
  // we do not have to free this
  FastEvaluator *eval;

  // parameters
  const OptimizerParams &params;

  SimpleRng rng;

  void SetRemoveRepeats(bool new_r);

  void Anneal(SmartPointer<Individual> old, SmartPointer<Individual> nw,
              double temperature);

public:
  MutationJob() = delete;
  MutationJob(const OptimizerParams &params, FastEvaluator *eval)
      : params(params), eval(eval), remove_repeats(false),
        hashes(new unordered_set<unsigned long long>), rng() {}
  ~MutationJob() {}

  void Message(ThreadJobMessage msg) override;

  void RunCycle(SmartPointer<PopulationBase> *pop_in,
                SmartPointer<PopulationBase> *pop_out) override;
};