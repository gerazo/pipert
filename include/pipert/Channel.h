#ifndef _CHANNEL_H_
#define _CHANNEL_H_

#include <cassert>
#include <utility>
#include "pipert/ChannelBase.h"
#include "pipert/Packet.h"
#include "pipert/PacketToFill.h"
#include "pipert/PacketToProcess.h"

namespace pipert {

/// A Channel receiving data of type T into its buffer through
/// `Packet<T>` objects.
///
/// A Channel is a circular buffer of `Packet<T>` objects which are used to
/// pass data around in the DSP pipeline.
/// Channels are always owned by a Scheduler object, they do not exist alone.
/// They are always connected to a _Node_ object which receives data by them.
/// Usually the _Node_ object is the one which creates these by using
/// Scheduler::CreateScheduledChannel() or Scheduler::CreatePolledChannel().
/// Anyone who has access to a Channel can send data through it by acquiring a
/// Packet and filling it up.
///
/// A Channel has a fixed size buffer which is created at _set up_ time.
/// No memory allocation is done during _run-time_ in the system.
///
/// Packets filled up with data can either be
/// - Automatically shceduled for processing (job) by a worker thread.
///   See ScheduledChannel for details.
/// - Manually polled for new data by your own, custom thread.
///   See PolledChannel for details.
///
/// In either case, the order of scheduling is respected
/// (earliest deadline first).
///
/// \tparam T User supplied data type that is used as the data
///           received by this Channel.
///           `Packet<T>` objects will be created in the Channel buffer to
///           facilitate the passing of data and scheduling of jobs on them.
template <class T>
class Channel : public ChannelBase {
 public:
  /// Acquire an empty Packet to be filled up with data.
  ///
  /// The Packet will be newly created in-place in the Channel buffer.
  /// If the data sender is ready by filling up the Packet, the discarded
  /// `PacketToFill<T>` object will automatically queue this Packet for
  /// processing / scheduling (RAII).
  /// An alternative option is to use PacketToFill<T>::Push() to force
  /// early queuing before stub destruction.
  ///
  /// \param client_name Name of _Node_, sender object or data source for
  ///                    identification purposes in logs and monitoring.
  /// \param timestamp Timestamp for the new Packet.
  ///                  See PacketBase::timestamp() for details.
  /// \param args Constructor parameters of type T. T will be
  ///             created in-place by passing these to its constructor.
  /// \return PacketToFill _stub_  pointing to the created Packet or
  ///         an empty object (see PacketStub::IsEmpty()) if there is
  ///         no free Packet available in the buffer.
  ///         The latter case refers to a scaling problem.
  template <class... Args>
  PacketToFill<T> Acquire(const char* client_name, Timer::Time timestamp,
                          Args&&... args);

  /// Sends the filled Packet into the queue for processing.
  /// Same as PacketToFill<T>::Push().
  void Push(PacketToFill<T>* filled_packet);

  /// Frees up the Packet after being done with its processing.
  /// Same as PacketToProcess<T>::Release().
  void Release(PacketToProcess<T>* processed_packet);

 protected:
  /// A Channel can only be created directly from the implementation.
  /// Call Scheduler::CreateScheduledChannel() or
  /// Scheduler::CreatePolledChannel() to create one.
  Channel(ChannelImpl* impl);
};

template <class T>
Channel<T>::Channel(ChannelImpl* impl)
  : ChannelBase(impl) {}

template <class T> template <class... Args>
PacketToFill<T> Channel<T>::Acquire(const char* client_name, Timer::Time timestamp,
                                    Args&&... args) {
  Packet<T>* new_packet = reinterpret_cast<Packet<T>*>(AcquireBase(client_name));
  assert(new_packet);
  new(new_packet) Packet<T>(timestamp, std::forward<Args>(args)...);
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