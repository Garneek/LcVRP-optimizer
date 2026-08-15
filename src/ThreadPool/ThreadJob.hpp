#pragma once

#include "Population/PopulationBase.hpp"
#include "SmartPointer/SmartPointer.hpp"

enum class ThreadJobMessage {
  MutationJobRemoveRepeatsOn,
  MutationJobRemoveRepeatsOff
};

class ThreadJob {
public:
  virtual ~ThreadJob() = 0;
  virtual void RunCycle(SmartPointer<PopulationBase> *in,
                        SmartPointer<PopulationBase> *out) = 0;
  virtual void Message(ThreadJobMessage msg) = 0;
};

inline ThreadJob::~ThreadJob() {};
