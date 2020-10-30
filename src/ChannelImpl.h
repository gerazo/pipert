#ifndef _CHANNELIMPL_H_
#define _CHANNELIMPL_H_

#include <cstdint>
#include <vector>
#include "AdaptiveSpinLock.h"
#include "pipert/ChannelBase.h"

namespace pipert {

class SchedulerImpl;

/// (_Part of internal implementation._)
///
/// This is the implementation of the Channel logic.
/// This template free code has all necessary data extracted from
/// the templated public interface.
/// Every kind of Channel object has a ChannelImpl backing it up.
///
/// For an overview of the packet processing workflow, see the following
/// diagrams.
///
/// \image html channel_creation_2.svg width=\textwidth
/// \image html processing_packets_1.svg width=\textwidth
/// \image html processing_packets_2.svg width=\textwidth
/// \image html processing_packets_3.svg width=\textwidth
/// \image html processing_packets_4.svg width=\textwidth
/// \image html processing_packets_5.svg width=\textwidth

class ChannelImpl {
 public:
  /// Real construction of Channel implementation.
  ///
  /// \param name See Scheduler::CreateScheduledChannel().
  /// \param capacity See Scheduler::CreateScheduledChannel().
  /// \param packet_size Extracted from `sizeof(Packet<T>)`.
  /// \param single_thread_object See Scheduler::CreateScheduledChannel().
  /// \param callback Pointer to the callback translator.
  ///        See Scheduler::CreateScheduledChannel() for the original callback.
  /// \param scheduler The owning SchedulerImpl of this ChannelImpl.
  ChannelImpl(const char* name, int capacity, int packet_size,
              void* single_thread_object,
              ChannelBase::InternalCallback callback, SchedulerImpl* scheduler);

  /// Destruction.
  ///
  /// The ChannelImpl is created and destroyed in sync with a Channel.
  ~ChannelImpl();

  ChannelImpl(const ChannelImpl&) = delete;
  ChannelImpl& operator=(const ChannelImpl&) = delete;

  /// Same as ChannelBase::GetCapacity().
  int GetCapacity() const { return capacity_; }

  /// Same as ChannelBase::GetPacketSize().
  int GetPacketSize() const { return packet_size_; }

  /// Same as ChannelBase::GetName().
  const char* GetName() const { return name_; }

  /// Get the key of mutual exclusion (monitor object) for the job queue.
  ///
  /// \return `this` pointer for stateful objects,
  ///         `nullptr` for stateless objects.
  ///         See Scheduler::CreateScheduledChannel() for details.
  void* GetState() const { return single_thread_object_; }

  /// Acquire a raw packet.
  /// See Channel::Acquire().
  PacketBase* Acquire(const char* client_name);

  /// Push a packet to be queued for processing.
  /// See Channel::Push().
  void Push(PacketBase* packet);

  /// Get the next packet queued for processing.
  /// \return Pointer the Packet or `nullptr` on empty queue.
  /// See PolledChannel::Poll().
  PacketBase* GetNext();

  /// Release a packet after processing.
  /// See Channel::Release().
  void Release(PacketBase* packet);

  /// Gets timestamp of next packet in job queue.
  Timer::Time PeekNext() const;

  /// Gets and removes next packet from job queue.
  PacketBase* PopNext();

  /// Sends the packet for execution.
  void Execute(PacketBase* packet);

  /// See ChannelBase::GetFreeBufferLength()
  int GetFreeBufferLength();

  /// See ChannelBase::GetQueuedBufferLength()
  int GetQueuedBufferLength();

  /// Sets the public interface objects.
  /// It allows the interface to be moved.
  void SetBase(ChannelBase* base);

 private:
  /// Ordering between packets based on timestamp.
  struct PacketOrdering {
    bool operator()(PacketBase* a, PacketBase* b);
  };

  /// \return True if this Channel is a scheduled one, false if polled.
  bool IsScheduled() const { return callback_; }

  /// \return Mutex of the job queue.
  ///         It is local with PolledChannel,
  ///         it is the Scheduler level mutex with ScheduledChannel.
  AdaptiveSpinLock& GetQueuedMutex();

  /// Drops a packet from the job queue if it can.
  /// \return True if a packet was dropped.
  bool TryDroppingPacket();

  /// Monitor object, See Scheduler::CreateScheduledChannel().
  void* single_thread_object_;

  /// See ChannelImpl::ChannelImpl() for details.
  ChannelBase::InternalCallback callback_;

  /// The raw buffer memory area of this Channel.
  int8_t* pool_;

  /// Local mutex for the free packet queue.
  AdaptiveSpinLock free_mutex_;

  /// Container for free packets.
  /// These are the alread released packets, not the ones in transit.
  /// (In transit ones are in PacketToFill and PacketToProcess objects.)
  /// This is a stack used from its end.
  std::vector<PacketBase*> free_packets_;

  /// Local mutex for the job (to-be-processed packets) queue.
  /// A global one is used with ScheduledChannel as in this case,
  /// every modification to the job queue has to be synced with the Scheduler.
  AdaptiveSpinLock queued_mutex_;

  /// Job (to-be-processed packets) queue.
  /// This is a heap having the oldest timestamp on top.
  std::vector<PacketBase*> queued_packets_;

  const char* name_;          ///< See GetName().
  int capacity_;              ///< See GetCapacity().
  int packet_size_;           ///< See GetPacketSize().
  SchedulerImpl* scheduler_;  ///< The connected Scheduler implementation.
  ChannelBase* base_;         ///< See SetBase().
};

}  // namespace pipert

#endif  //_CHANNELIMPL_H_