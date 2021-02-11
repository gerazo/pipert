#include "SchedulerImpl.h"

#include <cassert>
#include <iostream>
#include <mutex>

#include "ChannelImpl.h"
#include "ProfileData.h"
#include "ProfilerImpl.h"
#include "ThreadId.h"

namespace pipert {

SchedulerImpl::SchedulerImpl(int workers_number, ProfilerImpl* profiler)
    : state_queue_(StateOrdering(&state2channel_queues_)),
      keep_running_(false),
      running_(false),
      workers_number_(workers_number),
      profiler_(profiler) {
  assert(workers_number > 0);
  workers_.reserve(workers_number);
}

SchedulerImpl::~SchedulerImpl() {
  if(running_.load(std::memory_order_acquire))
    Stop();
  assert(channels_.size() == 0);
}

void SchedulerImpl::RegisterChannel(ChannelImpl* channel) {
  assert(!running_.load(std::memory_order_acquire));
  channels_.push_back(channel);
  void* state = channel->GetState();
  // for every state we reserve the size of the number of channels belonging to
  // that state
  auto it = state2channel_queues_.find(state);
  if (it != state2channel_queues_.end()) {
    it->second.heap.reserve(it->second.heap.capacity() + 1);
  } else {  // new state found
    state_queue_.reserve(state_queue_.capacity() + 1);
    state2channel_queues_[state].heap.reserve(1);
  }
  if (profiler_) {
    ProfileData* data = new ProfileData(channel->GetName());
    channel->SetProfileData(data);
  }
}

void SchedulerImpl::UnregisterChannel(ChannelImpl* channel) {
  assert(!running_.load(std::memory_order_acquire));
  auto it = std::find(channels_.begin(), channels_.end(), channel);
  assert(it != channels_.end());
  if (profiler_) {
    assert(channel->GetProfileData());
    delete channel->GetProfileData();
    channel->SetProfileData(nullptr);
  }
  *it = channels_.back();
  channels_.pop_back();
}

void SchedulerImpl::Start() {
  assert(!running_.load(std::memory_order_acquire));
  if(running_.load(std::memory_order_acquire))
    return;
  running_.store(true, std::memory_order_release);
  keep_running_.store(true, std::memory_order_release);
  int i = workers_number_;
  while (i-- > 0) {
    workers_.emplace_back(&SchedulerImpl::RunTasks, this);
  }
  if (profiler_) profiler_->Start();
}

void SchedulerImpl::Stop() {
  assert(running_.load(std::memory_order_acquire));
  if(!running_.load(std::memory_order_acquire))
    return;
  {
    std::unique_lock<AdaptiveSpinLock> lock(global_mutex_);
    keep_running_.store(false, std::memory_order_release);
  }
  global_covar_.notify_all();
  for (auto& t : workers_) {
    t.join();
  }
  running_.store(false, std::memory_order_release);
  workers_.clear();
  if (profiler_) profiler_->Stop();
}

void SchedulerImpl::JobsArrived(ChannelImpl* channel) {
  // No locking as it is called from ChannelImpl or here
  void* state = channel->GetState();
  auto& chq = state2channel_queues_[state];
  assert(chq.heap.count(channel) == 0);
  Timer::Time earliest =
      chq.heap.empty() ? Timer::kMaxTime : chq.heap.front()->PeekNext();
  chq.heap.push_heap(channel);
  if (chq.enabled && chq.heap.front()->PeekNext() < earliest) {
    if (earliest != Timer::kMaxTime) {
      state_queue_.force_pop_heap(state);
    }
    assert(state_queue_.count(state) == 0);
    state_queue_.push_heap(state);
    global_covar_.notify_one();
  }
}

void SchedulerImpl::JobsUpdated(ChannelImpl* channel, bool was_push) {
  // No locking as it is called from ChannelImpl or here
  void* state = channel->GetState();
  auto& chq = state2channel_queues_[state];
  assert(!chq.heap.empty());
  assert(chq.heap.count(channel) == 1);
  bool was_top = (chq.heap.front() == channel);
  Timer::Time earliest = chq.heap.front()->PeekNext();
  chq.heap.force_pop_heap(channel);
  chq.heap.push_heap(channel);
  if (chq.enabled && (chq.heap.front()->PeekNext() != earliest || was_top)) {
    state_queue_.force_pop_heap(state);
    assert(state_queue_.count(state) == 0);
    state_queue_.push_heap(state);
    if (was_push) {
      global_covar_.notify_one();
    }
  }
}

void SchedulerImpl::JobsDropped(ChannelImpl* channel) {
  // No locking as it is called from ChannelImpl or here
  void* state = channel->GetState();
  auto& chq = state2channel_queues_[state];
  assert(!chq.heap.empty());
  assert(chq.heap.count(channel) == 1);
  bool was_top = (chq.heap.front() == channel);
  chq.heap.force_pop_heap(channel);
  if (chq.enabled && was_top) {
    state_queue_.force_pop_heap(state);
    assert(state_queue_.count(state) == 0);
    if (!chq.heap.empty()) {
      state_queue_.push_heap(state);
    }
  }
}

bool SchedulerImpl::ChannelOrdering::operator()(const ChannelImpl* a,
                                                const ChannelImpl* b) {
  return a->PeekNext() > b->PeekNext();
}

SchedulerImpl::StateOrdering::StateOrdering(
    State2ChannelHeap* state2channel_queues)
    : state2channel_queues_(state2channel_queues) {}

bool SchedulerImpl::StateOrdering::operator()(void* a, void* b) {
  assert(state2channel_queues_->count(a) == 1);
  assert(state2channel_queues_->count(b) == 1);
  assert(!state2channel_queues_->at(a).heap.empty());
  assert(!state2channel_queues_->at(b).heap.empty());
  return ChannelOrdering()(state2channel_queues_->at(a).heap.front(),
                           state2channel_queues_->at(b).heap.front());
}

void SchedulerImpl::RunTasks() {
  ThreadId::TagCurrentThread();
  while (keep_running_.load(std::memory_order_acquire)) {
    void* state = nullptr;
    ChannelImpl* channel = nullptr;
    PacketBase* packet = nullptr;
    {
      std::unique_lock<AdaptiveSpinLock> lock(global_mutex_);
      while (keep_running_.load(std::memory_order_acquire) &&
             state_queue_.empty()) {
        global_covar_.wait(lock);
      }
      if (!state_queue_.empty()) {
        state = state_queue_.front();
        assert(state2channel_queues_.count(state) == 1);
        auto& chq = state2channel_queues_[state];
        assert(chq.enabled);
        assert(!chq.heap.empty());
        channel = chq.heap.front();
        assert(channel->GetState() == state);
        // state == nullptr has the stateless channels, so disabling is needed
        if (state != nullptr) {
          // set stateful channels for this state to disabled
          chq.enabled = false;
          state_queue_.pop_heap();
        }
        packet = channel->PopNext();
        assert(packet);
      }
    }
    if (packet) {
      assert(channel);
      ThreadId::SetNameOfCurrentThread(channel->GetName());
      channel->Execute(packet);
      if (state != nullptr) {
        // set stateful channels for this state to enabled again
        std::lock_guard<AdaptiveSpinLock> lock(global_mutex_);
        assert(state_queue_.count(state) == 0);
        assert(state2channel_queues_.count(state) == 1);
        auto& chq = state2channel_queues_[state];
        assert(!chq.enabled);
        chq.enabled = true;
        if (!chq.heap.empty()) {
          state_queue_.push_heap(state);
          // we do not need global_covar_.notify_one() here as this thread goes
          // on by servicing this state
        }
      }
    }
  }
}

}  // namespace pipert