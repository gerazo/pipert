#ifndef _SCHEDULER_H_
#define _SCHEDULER_H_

#include "pipert/PolledChannel.h"
#include "pipert/ScheduledChannel.h"

namespace pipert {

class SchedulerImpl;

/// The main object of a pipeline owning all Channel objects and
/// being responsible for scheduling.
///
/// One Scheduler object governs a complete DSP pipeline and all operations,
/// data structures connected to it.
/// Scheduler runs a real-time system when it is in its _running state_,
/// so no memory allocations are done during the operation of the pipeline.
/// This means that any configuration including creating Channel objects
/// should be done before setting the pipleine into a the _running_ state by
/// using the Start() method.
///
/// Scheduler keeps up a fixed number of _worker threads_ which are used to
/// execute processing functions of _nodes_ connected to the receiver side
/// of ScheduledChannel objects.
/// The scheduling is done by minimizing the delay between useful work units
/// and also minimizing the amount of time a Packet spends in the pipeline.
///
/// During the operation of the pipeline, no memory is allocated.
/// All Channel objects use a fixed, preconfigured size buffer where Packet
/// objects stay in place.
/// This way, buffers are not moved in memory and transforming operations
/// push the data forward from one buffer to an other one residing in the
/// next Channel in the pipeline.
///
/// There is an option to connect your own, custom threads to the pipeline by
/// using a PolledChannel.
/// A PolledChannel has all the buffering functionality but the Packet objects
/// in its buffer are not subject to scheduling done by the Scheduler.
/// Instead, the user can poll the buffer from a custom thread.
/// This can be used to implement _UI threads_ or creating _exit points_ from
/// the pipeline transferring data to an other system.
///
/// A Scheduler runned pipeline is really flexible. You can have arbitrary
/// number of _entry and exit points_ and also _feedback loops_ in the chain.
/// Until timestamps are correct, scheduling is done accordingly.
///
/// To ease creating and changing pipelines during development,
/// the structure of a pipeline is not stored by the system.
/// Instead, direct connections and references to Channel objects from user
/// supplied _nodes_ are governing the direction of data.
/// However, this data flow can be traced and measured nicely by the built-in
/// monitoring system.
///
/// You can have multiple Scheduler objects running in parallel.
/// This means you will run more, completely independent pipelines together.
/// However, you should keep in mind that these will fight for the same CPU
/// resources despite being isolated in memory.
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