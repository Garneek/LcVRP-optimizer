#include "ThreadData.hpp"
#include "MutationJob.hpp"
#include "ThreadJob.hpp"

ThreadData::ThreadData(const OptimizerParams &params, FastEvaluator *eval)
    : params(params), job(new MutationJob(params, eval)) {
  join_flag = false;
  running_threads = 0;
  iteration_running_flag = false;
  pop_in = nullptr;
  pop_out = nullptr;
  this->eval = eval;
}

ThreadData::ThreadData(ThreadData &&other)
    : params(other.params), job(other.job), pop_in(other.pop_in),
      pop_out(other.pop_out) {
  other.job = nullptr;
  join_flag = static_cast<bool>(other.join_flag);
  running_threads = static_cast<int>(other.running_threads);
  iteration_running_flag = static_cast<bool>(other.iteration_running_flag);
  other.pop_in = nullptr;
  other.pop_out = nullptr;
  this->eval = other.eval;
  other.eval = nullptr;
}

void ThreadData::SetRemoveRepeats(bool new_r) {
  job->Message(new_r ? ThreadJobMessage::MutationJobRemoveRepeatsOn
                     : ThreadJobMessage::MutationJobRemoveRepeatsOff);
}

void ThreadData::StartIteration(SmartPointer<PopulationBase> *in,
                                SmartPointer<PopulationBase> *out) {
  pop_in = in;
  pop_out = out;
  iteration_running_flag = true;
}

void ThreadData::FreePointers() {
  delete pop_in;
  delete pop_out;
  pop_in = nullptr;
  pop_out = nullptr;
}