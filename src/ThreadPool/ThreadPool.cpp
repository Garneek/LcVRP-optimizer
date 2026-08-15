#include "ThreadPool.hpp"

#include <chrono>
#include <thread>
#include <vector>

ThreadPool::ThreadPool(const OptimizerParams &params, FastEvaluator &eval)
    : thread_data(params, &eval), params(params) {}

ThreadPool::ThreadPool(ThreadPool &&other)
    : params(other.params), thread_data(std::move(other.thread_data)) {
  thread_handles = std::move(other.thread_handles);
}

ThreadPool::~ThreadPool() {
  thread_data.Join();
  for (int i = 0; i < thread_handles.size(); i++)
    thread_handles[i].join();
}

void ThreadPool::InitThreads() {
  for (int i = 0; i < params.N_THREADS_PER_POP; i++)
    thread_handles.push_back(thread(&ThreadData::ThreadLoop, &thread_data));
}

void ThreadPool::StartMutation(SmartPointer<PopulationBase> &pop,
                               SmartPointer<PopulationBase> &new_pop,
                               bool remove_repeats) {
  thread_data.SetRemoveRepeats(remove_repeats);

  thread_data.StartIteration(new SmartPointer<PopulationBase>(pop),
                             new SmartPointer<PopulationBase>(new_pop));
}

void ThreadPool ::AwaitFinish() {
  int target_size = thread_data.GetInputPopSize();
  int current_size = 0;
  do {
    current_size = thread_data.GetOutputPopSize();

    this_thread::sleep_for(chrono::microseconds(10));
  } while (current_size < target_size);

  thread_data.FinishIteration();

  while (thread_data.GetRunningThreads() > 0)
    this_thread::sleep_for(chrono::microseconds(10));

  thread_data.FreePointers();
}
