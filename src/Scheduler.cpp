#include "pipert/Scheduler.h"

#include <cassert>
#include <thread>
#include <utility>

#include "ChannelImpl.h"
#include "SchedulerImpl.h"
#include "pipert/PacketStub.h"

namespace pipert {

Scheduler::Scheduler(int workers, Profiler&& profiler)
    : profiler_(std::move(profiler)) {
  impl_ = new SchedulerImpl(
      workers <= 0 ? std::thread::hardware_concurrency() : workers,
      profiler_.impl_);
}

Scheduler::~Scheduler() {
  assert(impl_);
  delete impl_;
  impl_ = nullptr;
}

void Scheduler::Start() {
  assert(impl_);
  impl_->Start();
}

void Scheduler::Stop() {
  assert(impl_);
  impl_->Stop();
}

bool Scheduler::IsRunning() {
  assert(impl_);
  return impl_->IsRunning();
}

int Scheduler::GetWorkerNumber() {
  assert(impl_);
  return impl_->GetWorkerNumber();
}

Profiler& Scheduler::GetProfiler() {
  return profiler_;
}

ChannelImpl* Scheduler::CreateChannelImpl(
    const char* name, int capacity, int packet_size, void* single_thread_object,
    ChannelBase::InternalCallback callback) {
  assert(impl_);
  ChannelImpl* ch_impl = new ChannelImpl(name, capacity, packet_size,
                                         single_thread_object, callback, impl_);
  return ch_impl;
}

}  // namespace pipert