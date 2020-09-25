#ifndef _SCHEDULER_IMPL_H_
#define _SCHEDULER_IMPL_H_

#include <algorithm>
#include <atomic>
#include <cassert>
#include <condition_variable>
#include <thread>
#include <unordered_map>
#include <utility>
#include <vector>

#include "AdaptiveSpinLock.h"
#include "ChannelImpl.h"

namespace pipert {

class SchedulerImpl {
 public:
  SchedulerImpl(int workers_number);
  ~SchedulerImpl();
  SchedulerImpl(const SchedulerImpl&&) = delete;
  SchedulerImpl& operator=(const SchedulerImpl&&) = delete;

  // Use these in configuration phase before running
  void RegisterChannel(ChannelImpl* channel);
  void UnregisterChannel(ChannelImpl* channel);

  void Start();  // Enter running phase
  void Stop();   // Leave runnig phase
  bool IsRunning() { return running_.load(std::memory_order_acquire); }

  AdaptiveSpinLock& GetMutex() { return global_mutex_; }
  void JobsArrived(ChannelImpl* channel);
  void JobsUpdated(ChannelImpl* channel, bool was_push);
  void JobsDropped(ChannelImpl* channel);

 private:
  using ChannelHeap = std::vector<ChannelImpl*>;
  struct EnabledChannelHeap {
    EnabledChannelHeap() : enabled(true) {}
    bool enabled;
    ChannelHeap heap;
  };
  using State2ChannelHeap = std::unordered_map<void*, EnabledChannelHeap>;
  using StateHeap = std::vector<void*>;

  struct ChannelOrdering {
    bool operator()(ChannelImpl* a, ChannelImpl* b);
  };

  struct StateOrdering {
    State2ChannelHeap* state2channel_queues_;
    StateOrdering(State2ChannelHeap* state2channel_queues);
    bool operator()(void* a, void* b);
  };

  template <class T>
  static void MoveToTopInHeap(std::vector<T>& heap, T value);

  void RunTasks();

  std::vector<ChannelImpl*> channels_;
  AdaptiveSpinLock global_mutex_;
  std::condition_variable_any global_covar_;
  StateHeap state_queue_;
  State2ChannelHeap state2channel_queues_;
  std::vector<std::thread> workers_;
  std::atomic_bool keep_running_;
  std::atomic_bool running_;
  int workers_number_;
};

template <class T>
void SchedulerImpl::MoveToTopInHeap(std::vector<T>& heap, T value) {
  auto it = std::find(heap.begin(), heap.end(), value);
  assert(it != heap.end());
  int i = &*it - &heap[0];
  while (i > 0) {
    int j = (i - 1) / 2;
    assert(j < i);
    std::swap(heap[j], heap[i]);
    i = j;
  }
}

}  // namespace pipert

#endif  //_SCHEDULER_IMPL_H_