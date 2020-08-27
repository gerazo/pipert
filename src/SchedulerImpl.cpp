#include "SchedulerImpl.h"

#include <algorithm>
#include "ChannelImpl.h"

namespace pipert {

SchedulerImpl::SchedulerImpl(int workers_number)
  : workers_number_(workers_number) {
  workers_.reserve(workers_number);
}

SchedulerImpl::~SchedulerImpl() {
  Stop();
  assert(channels_.size() == 0);
}

void SchedulerImpl::Start() {
  keep_running_.store(true, std::memory_order_release);
  int i = workers_number_;
  while (i-- > 0) {
    workers_.emplace_back(&SchedulerImpl::RunTasks, this);
  }
}

void SchedulerImpl::Stop() {
  keep_running_.store(false, std::memory_order_release);
  for (auto& t : workers_) {
    t.join();
  }
  workers_.clear();
}

void SchedulerImpl::RegisterChannel(ChannelImpl* channel) {
  // TODO check that threads are not running
  channels_.push_back(channel);
  void* state = channel->GetState();
  state2channel_queues_[state];
}

void SchedulerImpl::UnregisterChannel(ChannelImpl* channel) {
  // TODO check that threads are not running
  auto it = std::find(channels_.begin(), channels_.end(), channel);
  assert(it != channels_.end());
  *it = channels_.back();
  channels_.pop_back();
}

void SchedulerImpl::JobArrived(ChannelImpl* /*channel*/) {
  // TODO Refresh this channel state
}

void SchedulerImpl::JobDropped(ChannelImpl* /*channel*/) {
  // TODO Refresh this channel state
}

bool SchedulerImpl::ChannelOrdering::operator()(ChannelImpl* a, ChannelImpl* b) {
  return a->PeekNext() > b->PeekNext();
}

SchedulerImpl::StateOrdering::StateOrdering(State2ChannelHeap* state2channel_queues)
  : state2channel_queues_(state2channel_queues) {}

bool SchedulerImpl::StateOrdering::operator()(void* a, void* b) {
  assert(state2channel_queues_->count(a) == 1);
  assert(state2channel_queues_->count(b) == 1);
  assert(!state2channel_queues_->at(a).empty());
  assert(!state2channel_queues_->at(b).empty());
  return ChannelOrdering()(state2channel_queues_->at(a).front(), state2channel_queues_->at(b).front());
}

void SchedulerImpl::RunTasks() {
  while (keep_running_.load(std::memory_order_acquire)) {
    // TODO
  }
}

}  // namespace pipert