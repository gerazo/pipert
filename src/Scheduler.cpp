#include "pipert/Scheduler.h"
#include "pipert/PacketStub.h"
#include "ChannelImpl.h"
#include "SchedulerImpl.h"

namespace pipert {

Scheduler::Scheduler(int workers) : impl_(new SchedulerImpl(workers)) {}

Scheduler::~Scheduler() { delete impl_; }

void Scheduler::Start() { impl_->Start(); }

void Scheduler::Stop() { impl_->Stop(); }

ChannelImpl* Scheduler::CreateChannelImpl(char* name, int capacity,
    int packet_size, void* mutex_state, ChannelBase::InternalCallback callback) {
  return new ChannelImpl(name, capacity, packet_size, mutex_state, callback, impl_);
}

}  // namespace pipert