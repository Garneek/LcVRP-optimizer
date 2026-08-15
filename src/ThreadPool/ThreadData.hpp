#pragma once
#include "OptimizerParams.hpp"
#include "Population/PopulationBase.hpp"
#include "SmartPointer/SmartPointer.hpp"
#include "ThreadJob.hpp"
#include <atomic>
#include <mutex>
#include <thread>

using namespace std;

class ThreadData {
private:
  // Waiting for threads to stop
  atomic_bool join_flag;

  // is some population currently being mutated
  atomic_bool iteration_running_flag;

  // Population to be mutated
  SmartPointer<PopulationBase> *pop_in;
  // Mutated population output
  SmartPointer<PopulationBase> *pop_out;
  // Number of threads still computing
  atomic_int running_threads;

  // Eval is thread safe evaluator + lookup table optimization, no need to free
  // this ptr is borrowed from optimizer
  FastEvaluator *eval;

  mutex job_guard;
  ThreadJob *job;

  // parameters
  const OptimizerParams &params;

public:
  ThreadData() = delete;
  ThreadData(const ThreadData &other) = delete;
  ThreadData(ThreadData &&other);

  ThreadData(const OptimizerParams &params, FastEvaluator *eval);
  ~ThreadData() { delete job; }

  void SetRemoveRepeats(bool new_r);

  void StartIteration(SmartPointer<PopulationBase> *in,
                      SmartPointer<PopulationBase> *out);
  void FinishIteration() { iteration_running_flag = false; }
  void FreePointers();
  int GetRunningThreads() { return running_threads; }

  int GetInputPopSize() { return (*pop_in)->GetPopSize(); }
  int GetOutputPopSize() { return (*pop_out)->GetPopSize(); }

  void Join() {
    FinishIteration();
    join_flag = true;
  }

  void ThreadLoop() {
    bool was_running = false;

    while (!join_flag) {
      // cout << "Thread" << endl;
      while (iteration_running_flag) {
        if (!was_running) {
          was_running = true;
          running_threads++;
        }
        job->RunCycle(pop_in, pop_out);
      }

      if (was_running) {
        running_threads--;
        was_running = false;
      }

      this_thread::sleep_for(chrono::microseconds(10));
    }
  }
};