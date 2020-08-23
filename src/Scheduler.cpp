#include "pipert/Scheduler.h"
#include "pipert/SchedulerImpl.h"
#include "pipert/PacketStub.h"

namespace pipert {

Scheduler::Scheduler(int workers) : impl_(new SchedulerImpl(workers)) {}

Scheduler::~Scheduler() { delete impl_; }

void Scheduler::Start() { impl_->Start(); }

void Scheduler::Stop() { impl_->Stop(); }

}  // namespace pipert