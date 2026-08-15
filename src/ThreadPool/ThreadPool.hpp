#pragma once

#include "OptimizerParams.hpp"
#include "Population/PopulationBase.hpp"
#include "SmartPointer/SmartPointer.hpp"
#include "ThreadData.hpp"

#include <thread>
#include <vector>

class ThreadPool {
private:
  const OptimizerParams &params;
  ThreadData thread_data;

  std::vector<thread> thread_handles;

public:
  ThreadPool() = delete;
  ThreadPool(const ThreadPool &other) = delete;
  ThreadPool(ThreadPool &&other);
  ThreadPool(const OptimizerParams &params, FastEvaluator &eval);
  ~ThreadPool();

  void InitThreads();

  void StartMutation(SmartPointer<PopulationBase> &pop,
                     SmartPointer<PopulationBase> &new_pop,
                     bool remove_repeats);

  void AwaitFinish();
};
