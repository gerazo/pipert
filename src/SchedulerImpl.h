#ifndef _SCHEDULER_IMPL_H_
#define _SCHEDULER_IMPL_H_

#include <algorithm>
#include <atomic>
#include <cassert>
#include <condition_variable>
#include <functional>
#include <thread>
#include <unordered_map>
#include <utility>
#include <vector>

#include "AdaptiveSpinLock.h"
#include "SteadyHeap.h"

namespace pipert {

class ChannelImpl;
class ProfilerImpl;
class ReceiverBase;

/// (_Part of internal implementation._)
///
/// This is the implementation of the Scheduler logic.
/// Every Scheduler must have a SchedulerImpl behind it.
class SchedulerImpl {
 public:
  /// Constructor.
  ///
  /// \param workers_number See Scheduler::Scheduler().
  /// \param profiler Profiler instance of the functioning implementation,
  ///                 or null if there is no logging going on.
  SchedulerImpl(int workers_number, ProfilerImpl* profiler);

  /// Destructor.
  ///
  /// SchedulerImpl is created and destroyed in sync with Scheduler.
  ~SchedulerImpl();

  SchedulerImpl(const SchedulerImpl&&) = delete;
  SchedulerImpl& operator=(const SchedulerImpl&&) = delete;

  /// Register a Channel that was newly created.
  /// This is used in the configuration phase before running
  /// and is automatically called at the end of Channel construction.
  void RegisterChannel(ChannelImpl* channel);

  /// Unregister a Channel that is about the be destroyed.
  /// This is used in configuration phase before running,
  /// and is automatically called before the destruction of a Channel.
  void UnregisterChannel(ChannelImpl* channel);

  /// Add a Receiver that was newly created.
  /// This is used in the configuration phase before running
  /// and is automatically called at the end of Receiver construction.
  void AddReceiver(ReceiverBase* receiver);

  /// Enter the running state.
  /// See Scheduler::Start().
  void Start();

  /// Enter the stopped/preparation/configuration state.
  /// See Scheduler::Stop().
  void Stop();

  void SetStateInvalid();

  /// Return current state.
  /// See Scheduler::IsRunning().
  bool IsRunning() { return running_.load(std::memory_order_acquire); }

  /// Returns the number of worker threads.
  /// See Scheduler::GetWorkerNumber().
  int GetWorkerNumber() { return workers_number_; }

  /// Return the Scheduler-level mutex for job queues.
  /// This is also used by all ScheduledChannel objects connected.
  AdaptiveSpinLock& GetMutex() { return global_mutex_; }

  /// ChannelImpl signals new jobs in queue.
  /// \param channel The signalling ChannelImpl.
  void JobsArrived(ChannelImpl* channel);

  /// ChannelImpl signals updated jobs in queue.
  /// \param channel The signalling ChannelImpl.
  /// \param was_push True if the modification was a result of a new job,
  ///        false if the modification was a result of a dropped job.
  void JobsUpdated(ChannelImpl* channel, bool was_push);

  /// ChannelImpl signals dropped jobs in queue.
  /// \param channel The signalling ChannelImpl.
  void JobsDropped(ChannelImpl* channel);

 private:
  /// Certain heaps belonging to a channel are disabled because the channel
  /// of a stateful node has a thread currently executing a job.
  /// Until the job is done, no other threads are allowed to enter.
  struct EnabledChannelHeap {
    EnabledChannelHeap() : enabled(true), heap(ChannelOrdering()) {}
    bool enabled;
    /// A heap is used as it can work as an in-place queue.
    SteadyHeap<ChannelImpl*,
               std::function<bool(const ChannelImpl*, const ChannelImpl*)>>
        heap;
  };

  /// Monitor object to channel heap mapping.
  using State2ChannelHeap = std::unordered_map<void*, EnabledChannelHeap>;

  /// Ordering of channels based on earliest timestamp for ChannelHeap.
  struct ChannelOrdering {
    bool operator()(const ChannelImpl* a, const ChannelImpl* b);
  };

  /// Ordering of monitor objects based on the earliest timestamp for StateHeap.
  struct StateOrdering {
    State2ChannelHeap* state2channel_queues_;
    StateOrdering(State2ChannelHeap* state2channel_queues);
    bool operator()(void* a, void* b);
  };

  /// Worker thread code getting a new job and executing it.
  /// If there are no jobs available, it sleeps until there is new work.
  void RunTasks();

  std::vector<ChannelImpl*> channels_;        ///< All registered channels.
  std::vector<ReceiverBase*> receivers_;      ///< All receiver side objects.
  AdaptiveSpinLock global_mutex_;             ///< See GetMutex().
  std::condition_variable_any global_covar_;  ///< CV for waking up threads.
  /// Queue for states in timestamp order as a heap of monitor objects
  SteadyHeap<void*, std::function<bool(void*, void*)>> state_queue_;
  State2ChannelHeap state2channel_queues_;  ///< State to channel mapping.
  std::vector<std::thread> workers_;        ///< Worker thread pool.
  std::atomic_bool keep_running_;           ///< Tells threads to run or not.
  std::atomic_bool running_;  ///< Tells what state was reached by all threads.
  int workers_number_;        ///< Real number of worker threads.
  ProfilerImpl* profiler_;  ///< Implementation of the profiler if there is any.
  /// State of the Scheduler. See Protocol class for more details.
  bool valid_state = true;
};

}  // namespace pipert

#endif  //_SCHEDULER_IMPL_H_