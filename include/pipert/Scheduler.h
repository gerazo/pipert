#ifndef _SCHEDULER_H_
#define _SCHEDULER_H_

#include "pipert/PolledChannel.h"
#include "pipert/ScheduledChannel.h"

namespace pipert {

class SchedulerImpl;

/// The main object of a pipeline owning all Channels and
/// being responsible for scheduling.
class Scheduler {
 public:
  /// Creates a Scheduler object and the system enters preparation state.
  ///
  /// The _preparation_ or _set up_ state is for creating Channel objects
  /// and any kind of configuration which involves memory allocation or is
  /// otherwise slow.
  /// You can create channels by calling
  /// CreateScheduledChannel() or CreatePolledChannel().
  /// You can enter _running state_ by calling Start().
  ///
  /// \param workers Number of worker threads to be used for scheduling.
  ///                By default, it detects the number of CPUs in the system
  ///                and starts that many threads.
  Scheduler(int workers = 0);

  /// Destroys the object.
  ///
  /// \pre Scheduler should be in _preparation/stopped_ state, or this needs
  ///      to issue a complete stop of worker threads and wait for them to
  ///      shut down.
  /// \pre It is also a requirement that all Channel objects are destroyed
  ///      by this point to have a clean shutdown.
  ~Scheduler();

  Scheduler(const Scheduler&) = delete;
  Scheduler& operator=(const Scheduler&) = delete;

  /// Create a PolledChannel.
  ///
  /// This is the function to be used to create a PolledChannel.
  /// See PolledChannel for details.
  /// \tparam T See `Channel<T>` for details.
  /// \param name The preferably unique name of the to-be-created Channel
  ///             which will be used for identification in
  ///             logs, monitoring, debugging.
  ///             See ChannelBase::GetName() for details.
  /// \param capacity The number of Packet objects that the to-be-created
  ///                 Channel can hold.
  ///                 See ChannelBase::GetCapacity().
  template <class T>
  PolledChannel<T> CreatePolledChannel(const char* name, int capacity);

  /// Create a ScheduledChannel.
  ///
  /// This is the function to be used to create a ScheduledChannel.
  /// See ScheduledChannel for details.
  /// \tparam T See `Channel<T>` for details.
  /// \param name The preferably unique name of the to-be-created Channel
  ///             which will be used for identification in
  ///             logs, monitoring, debugging.
  ///             See ChannelBase::GetName() for details.
  /// \param capacity The number of Packet objects that the to-be-created
  ///                 Channel can hold.
  ///                 See ChannelBase::GetCapacity().
  /// \param single_thread_object This parameter determines the
  ///        mutual exclusion the Scheduler will employ when calling the
  ///        registered callback function.
  ///        This depends on the _Node_ which will be called.
  ///        There are 3 typical scenarios:
  ///        - For _stateless_ node objects, use `nullptr`, so all free
  ///          threads can call the registered callback simultaneously.
  ///          This will allow full parallelism processing multiple
  ///          packets simultaneously.
  ///          The drawback is that the called function (or Node object)
  ///          should only use local or `thread_local` resources
  ///          to avoid conflicting with itself.
  ///        - For _stateful_ nodes, give a unique pointer here,
  ///          practically your Node object's `this` pointer.
  ///          In this case, only one thread will enter your object code at
  ///          one single time, so you can use object member data without any
  ///          synchronization primitives.
  ///        - For _stateful_ nodes with multiple Channel objects, use
  ///          the same pointer in all to-be-created channels connecting to
  ///          the same object. (Practically the same this pointer.)
  ///          This way, from all Channel objects connected to your Node,
  ///          only one callback will be scheduled at the same time.
  ///          This implements mutual exclusion between all connected channels.
  ///          This also allows normal member data usage without parallel
  ///          primitives in case of complex Nodes having multiple input
  ///          channels.
  /// \param callback A callback function to be registered in the new Channel.
  ///                 This will be the one which will be called by the
  ///                 a worker thread of the Scheduler when a Packet
  ///                 in the Channel is scheduled for processing.
  template <class T>
  ScheduledChannel<T> CreateScheduledChannel(
      const char* name, int capacity, void* single_thread_object,
      typename ScheduledChannel<T>::Callback callback);

  /// Start all worker threads by entering _running state_.
  ///
  /// \pre We should be in _preparation/stopped_ state.
  ///      It is not possible to restart a Scheduler,
  ///      but it is possible to Stop and Start it again.
  /// \pre All Channel objects were created, capacities and references are in
  ///      in place to enter running state, so we do not have to do any
  ///      configuration during running.
  void Start();

  /// Sends a stop request to all worker threads and waits for them to
  /// shut down.
  ///
  /// \pre We should be in _running state_.
  ///      It is not possible to Stop a stopped Scheduler but it is possible
  ///      Start it and Stop it as many times as you want.
  ///      The corresponding thread pool is always rebuilt from scratch after
  ///      a clean shutdown.
  void Stop();

  /// Tells the current state of Scheduler (running or stopped/preparation).
  /// \return True if Scheduler is running.
  bool IsRunning();

 private:
  /// Part of internal implementation where a raw channel implementation is
  /// created from generic parameters covering both type of channels.
  ChannelImpl* CreateChannelImpl(const char* name, int capacity,
                                 int packet_size, void* single_thread_object,
                                 ChannelBase::InternalCallback callback);

  /// Pointer to internal implementation where this object calls through to.
  SchedulerImpl* impl_;
};

template <class T>
PolledChannel<T> Scheduler::CreatePolledChannel(const char* name,
                                                int capacity) {
  ChannelImpl* chimpl =
      CreateChannelImpl(name, capacity, sizeof(Packet<T>), nullptr, nullptr);
  return PolledChannel<T>(chimpl);
}

template <class T>
ScheduledChannel<T> Scheduler::CreateScheduledChannel(
    const char* name, int capacity, void* single_thread_object,
    typename ScheduledChannel<T>::Callback callback) {
  ChannelImpl* chimpl =
      CreateChannelImpl(name, capacity, sizeof(Packet<T>), single_thread_object,
                        &ScheduledChannel<T>::CallbackTranslator);
  return ScheduledChannel<T>(chimpl, callback);
}

}  // namespace pipert

#endif  //_SCHEDULER_H_