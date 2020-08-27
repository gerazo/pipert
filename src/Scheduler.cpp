#include "pipert/Scheduler.h"

#include <cassert>
#include "ChannelImpl.h"
#include "SchedulerImpl.h"
#include "pipert/PacketStub.h"

namespace pipert {

Scheduler::Scheduler(int workers) : impl_(new SchedulerImpl(workers)) {}

Scheduler::~Scheduler() {
  assert(impl_);
  delete impl_;
  impl_ = nullptr;
}

void Scheduler::Start() { impl_->Start(); }

void Scheduler::Stop() { impl_->Stop(); }

ChannelImpl* Scheduler::CreateChannelImpl(
    char* name, int capacity, int packet_size, void* single_thread_object,
    ChannelBase::InternalCallback callback) {
  return new ChannelImpl(name, capacity, packet_size, single_thread_object,
                         callback, impl_);
}

}  // namespace pipert