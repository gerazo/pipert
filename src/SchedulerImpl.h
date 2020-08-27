#ifndef _SCHEDULER_IMPL_H_
#define _SCHEDULER_IMPL_H_

#include <atomic>
#include <cassert>
#include <thread>
#include <unordered_map>
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

  void Start();
  void Stop();

  void RegisterChannel(ChannelImpl* channel);
  void UnregisterChannel(ChannelImpl* channel);

  AdaptiveSpinLock* GetMutex() { return &global_mutex_; }
  void JobArrived(ChannelImpl* channel);
  void JobDropped(ChannelImpl* channel);

 private:
  using ChannelHeap = std::vector<ChannelImpl*>;
  using State2ChannelHeap = std::unordered_map<void*, ChannelHeap>;
  using StateHeap = std::vector<void*>;

  struct ChannelOrdering {
    bool operator()(ChannelImpl* a, ChannelImpl* b);
  };

  struct StateOrdering {
    State2ChannelHeap* state2channel_queues_;
    StateOrdering(State2ChannelHeap* state2channel_queues);
    bool operator()(void* a, void* b);
  };

  std::vector<ChannelImpl*> channels_;
  AdaptiveSpinLock global_mutex_;
  StateHeap state_queue_;
  State2ChannelHeap state2channel_queues_;
  std::vector<std::thread> workers_;
  std::atomic_bool keep_running_;
  int workers_number_;

  void RunTasks();
};

}  // namespace pipert

#endif  //_SCHEDULER_IMPL_H_