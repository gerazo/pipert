#ifndef _CHANNEL_H_
#define _CHANNEL_H_

#include <cassert>
#include <utility>
#include "pipert/ChannelBase.h"
#include "pipert/Packet.h"
#include "pipert/PacketToFill.h"
#include "pipert/PacketToProcess.h"

namespace pipert {

/// A Channel represents storage for a single stage in the data processing
/// chain, and receives data of type T into its buffer through `Packet<T>`
/// objects.
///
/// A Channel is a circular buffer of `Packet<T>` objects which are used to
/// pass data around in the DSP pipeline.
/// This buffer is of a fixed size, allocated at _set up_ time.
/// No memory allocation is done during _run-time_ in the system.
///
/// Channels are always owned by a Scheduler object, they do not exist alone.
/// They are always connected to a _Node_ object which receives data through
/// them.
/// Usually the _Node_ object is the one that creates Channels using
/// Scheduler::CreateScheduledChannel() or Scheduler::CreatePolledChannel().
///
/// Packets filled up with data can either be
/// - Automatically scheduled for processing (job) by a worker thread.
///   See ScheduledChannel for details.
/// - Manually polled for new data by your own, custom thread.
///   See PolledChannel for details.
///
/// In either case, the order of scheduling is respected
/// (earliest deadline first).
///
/// Therefore, a Channel exposes the interface for:
/// - _Acquiring_ a PacketToFill by requesting a memory address from the packet
///   pool hidden in the implementation. Anyone who has access to a Channel can
///   send data through it by acquiring and filling up a Packet.
/// - _Pushing_ a PacketToFill to be queued for processing.
/// - _Releasing_ an already processed PacketToProcess from the packet pool to
///   be "freed" so its memory location can be reused.
///
/// \tparam T User supplied data type representing the data stored in the
///           `Packet<T>` objects that flow through the Channel buffer.
template <class T>
class Channel : public ChannelBase {
 public:
  /// Acquire an empty Packet from the pool to be filled up with data.
  ///
  /// The Packet will be created anew in-place in the Channel buffer.
  /// After the data sender is done filling up the Packet, the discarded
  /// `PacketToFill<T>` object will automatically queue this Packet for
  /// processing / scheduling (following RAII).
  /// See PacketToFill<T>::Push() for forcing early queueing of packets before
  /// the destruction of the PacketStub.
  ///
  /// \param client_name Name of _Node_, sender object or data source for
  ///                    identification purposes in logs and monitoring.
  /// \param timestamp The exact time when the PacketToFill was requested to be
  ///                  acquired. See PacketBase::timestamp() for details.
  /// \param args Constructor parameters of type T which will be
  ///             created in-place.
  /// \return PacketToFill _stub_  pointing to the created Packet or
  ///         an empty object (see PacketStub::IsEmpty()) if there is
  ///         no free space available for a Packet in the buffer.
  ///         The latter case refers to a scaling problem.
  template <class... Args>
  PacketToFill<T> Acquire(const char* client_name, Timer::Time timestamp,
                          Args&&... args);

  /// Push a filled packet to be queued for processing in this Channel.
  ///
  /// Normally called upon the destruction of a PacketToFill.
  /// Same as PacketToFill<T>::Push().
  ///
  /// \param filled_packet The filled PacketToFill that will be pushed to be
  ///                      queued for processing.
  void Push(PacketToFill<T>* filled_packet);

  /// Release a contained Packet from the packet queue of this Channel, freeing
  /// its position in the packet pool to be reused again.
  ///
  /// Cuts the connection to the Packet in the PacketToProcess.
  /// Explicitly destructs the data contained in the Packet.
  ///
  /// Normally called upon the destruction of a PacketToProcess.
  /// Same as PacketToProcess<T>::Release().
  ///
  /// \param processed_packet The processed PacketToProcess whose associated
  ///                         Packet will be released from the packet pool.
  void Release(PacketToProcess<T>* processed_packet);

 protected:
  /// Construct a Channel using an implementation that must be provided.
  ///
  /// A Channel can only be created directly in the system implementation.
  /// If you wish to create one, call Scheduler::CreateScheduledChannel() or
  /// Scheduler::CreatePolledChannel().
  ///
  /// \param impl The obligatorily provided hidden implementation of Channel.
  Channel(ChannelImpl* impl);
};

template <class T>
Channel<T>::Channel(ChannelImpl* impl) : ChannelBase(impl) {}

template <class T>
template <class... Args>
PacketToFill<T> Channel<T>::Acquire(const char* client_name,
                                    Timer::Time timestamp, Args&&... args) {
  Packet<T>* new_packet =
      reinterpret_cast<Packet<T>*>(AcquireBase(client_name));
  assert(new_packet);
  new (new_packet) Packet<T>(timestamp, std::forward<Args>(args)...);
  return PacketToFill<T>(new_packet, this);
}

template <class T>
void Channel<T>::Push(PacketToFill<T>* filled_packet) {
  assert(!filled_packet->IsEmpty());
  PushBase(filled_packet->GetPacket());
  filled_packet->SetEmpty();
}

template <class T>
void Channel<T>::Release(PacketToProcess<T>* processed_packet) {
  assert(!processed_packet->IsEmpty());
  Packet<T>* packet = processed_packet->GetPacket();
  processed_packet->SetEmpty();
  packet->~Packet();
  ReleaseBase(packet);
}

}  // namespace pipert

#endif  //_CHANNEL_H_