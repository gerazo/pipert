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
#include "pipert/MeasurmentBuffer.h"
#include "pipert/UDPConnection.h"


namespace pipert {

/// (_Part of internal implementation._)
///
/// This is the implementation of the Scheduler logic.
/// Every Scheduler must have a SchedulerImpl behind it.
class SchedulerImpl {
 public:
  /// Constructor.
  ///
  /// \param workers_number See Scheduler::Scheduler().
  SchedulerImpl(int workers_number);

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

  /// Enter the running state.
  /// See Scheduler::Start().
  void Start();

  /// Enter the stopped/preparation/configuration state.
  /// See Scheduler::Stop().
  void Stop();

  /// Return current state.
  /// See Scheduler::IsRunning().
  bool IsRunning() { return running_.load(std::memory_order_acquire); }

  /// Returns the number of worker threads.
  /// See Scheduler::GetWorkerNumber().
  int GetWorkerNumber() { return workers_number_; }

  ///Retuns Measurement Buffer
  pipert::MeasurmentBuffer GetMeasurementBuffer(){
    return measurement_buffer_;
  }

  ///set MeasurementBuffer
  void SetMeasurementsBuffer(pipert::MeasurmentBuffer measurment_buffer_to_be_set_){
    measurement_buffer_=measurment_buffer_to_be_set_;
    buffer_is_set=true;
  }



///Reutun if current MeasurementProfile is running or not
 bool IsMeasurementProfileBufferRunning(){return !buffering_is_stopped;}

  /// Start measurements buffering
 void StartMeasurementBuffering() {
   if (buffer_is_set && buffering_is_stopped) {
     measurement_buffer_.enableSending = true;
     measurement_buffer_.generateBufferJob();
   }
 }


 /// Stop measurements buffering
 void StopMeasurementBuffering(){
   if (buffer_is_set && !buffering_is_stopped){
     measurement_buffer_.enableSending=false;
     buffering_is_stopped=true;
   }
 }




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
  /// Heaps are used as they can work as in-place queues.
  using ChannelHeap = std::vector<ChannelImpl*>;

  /// Certain heaps belonging to a channel are disabled because the channel
  /// of a stateful node has a thread currently executing a job.
  /// Until the job is done, no other threads are allowed to enter.
  struct EnabledChannelHeap {
    EnabledChannelHeap() : enabled(true) {}
    bool enabled;
    ChannelHeap heap;
  };

  /// Monitor object to channel heap mapping.
  using State2ChannelHeap = std::unordered_map<void*, EnabledChannelHeap>;

  /// Heap of monitor objects.
  using StateHeap = std::vector<void*>;

  /// Ordering of channels based on earliest timestamp for ChannelHeap.
  struct ChannelOrdering {
    bool operator()(ChannelImpl* a, ChannelImpl* b);
  };

  /// Ordering of monitor objects based on the earliest timestamp for StateHeap.
  struct StateOrdering {
    State2ChannelHeap* state2channel_queues_;
    StateOrdering(State2ChannelHeap* state2channel_queues);
    bool operator()(void* a, void* b);
  };

  /// Special method to force something to the top of the heap.
  /// This is useful to remove and reinsert something in a heap when the
  /// item is modified.
  template <class T>
  static void MoveToTopInHeap(std::vector<T>& heap, T value);

  /// Worker thread code getting a new job and executing it.
  /// If there are no jobs available, it sleeps until there is new work.
  void RunTasks();

  std::vector<ChannelImpl*> channels_;        ///< All registered channels.
  AdaptiveSpinLock global_mutex_;             ///< See GetMutex().
  std::condition_variable_any global_covar_;  ///< CV for wakeing up threads.
  StateHeap state_queue_;  ///< Queue for states in timestamp order.
  State2ChannelHeap state2channel_queues_;  ///< State to channel mapping.
  std::vector<std::thread> workers_;        ///< Worker thread pool.
  std::atomic_bool keep_running_;           ///< Tells threads to run or not.
  std::atomic_bool running_;  ///< Tells what state was reached by all threads.
  int workers_number_;        ///< Real number of wroker threads.
  pipert::MeasurmentBuffer measurement_buffer_;  ///< measurements buffer
  bool buffering_is_stopped; ///<buffering is stopped
  bool buffer_is_set; ///<buffer is set
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