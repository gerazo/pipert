#include "SchedulerImpl.h"

#include <algorithm>
#include "ChannelImpl.h"
#include <iostream>

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
  state2channel_queues_[state].reserve(state2channel_queues_[state].capacity() + 1);
  std::cout << "Sch cap: " << state2channel_queues_[state].capacity() << std::endl;
  // for every state we reserve the size of the number of channels regarding to that state
}

void SchedulerImpl::UnregisterChannel(ChannelImpl* channel) {
  // TODO check that threads are not running
  auto it = std::find(channels_.begin(), channels_.end(), channel);
  assert(it != channels_.end());
  *it = channels_.back();
  channels_.pop_back();
}

void SchedulerImpl::JobArrived(ChannelImpl* channel) {
  // TODO Refresh this channel state
  std::lock_guard<AdaptiveSpinLock> lock(global_mutex_);
  std::cout << "Job arrived from Channel: " << channel->GetName() << std::endl;
  void* state = channel->GetState();
  assert(channel);
  // Putting the channel into the heap if it is not scheduled yet
  if (!channel->IsQueued()) {
    state2channel_queues_[state].push_back(channel);
    std::push_heap(state2channel_queues_[state].begin(), state2channel_queues_[state].end(), ChannelOrdering());
    channel->SetQueued(true);
    std::cout << "Channel added to queue" << std::endl;
  } else {
    // Reordering the heap if the channel is already in
    std::make_heap(state2channel_queues_[state].begin(), state2channel_queues_[state].end(), ChannelOrdering());
  }
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
    /*auto it = state2channel_queues_.begin();
    //assert(it);
    for(; it != state2channel_queues_.end(); ++it) {
      //std::lock_guard<AdaptiveSpinLock> lock(global_mutex_); // it seems it doesn't work correct
      m_.lock();
      if (!it->second.empty()) {
        // Getting the channel with the most fresh packet
        std::pop_heap(it->second.begin(), it->second.end(), ChannelOrdering());
        auto ch = it->second.back();
        it->second.pop_back();
        ch->SetQueued(false);
        ch->Execute();
      }
      m_.unlock();
    }*/
    m_.lock();
    if (!state2channel_queues_[nullptr].empty()) {
      std::pop_heap(state2channel_queues_[nullptr].begin(), state2channel_queues_[nullptr].end(), ChannelOrdering());
      auto ch = state2channel_queues_[nullptr].back();
      state2channel_queues_[nullptr].pop_back();
      ch->SetQueued(false);
      ch->Execute();
    }
    m_.unlock();
  }
}

}  // namespace pipert