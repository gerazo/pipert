#include "pipert/Scheduler.h"
#include "pipert/SchedulerImp.h"
#include "pipert/PacketStub.h"

namespace pipert {

Scheduler::Scheduler(int workers) : imp_(new SchedulerImp(workers)) {}

Scheduler::~Scheduler() { delete imp_; }

void Scheduler::Start() { imp_->Start(); }

void Scheduler::Stop() { imp_->Stop(); }

}  // namespace pipert