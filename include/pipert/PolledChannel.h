#ifndef _POLLEDCHANNEL_H_
#define _POLLEDCHANNEL_H_

#include "pipert/Channel.h"

namespace pipert {

/// A Channel whose packets can be manually received for processing by the
/// user's custom threads.
///
/// A PolledChannel is for using your own thread which is independent from the
/// scheduling done by the owning Scheduler object.
/// It can be useful in the following scenarios:
/// - You need a dedicated thread for certain actions (fixed thread affinity).
///   Using UI libraries or libraries which are not thread safe and need to be
///   called exclusively from one thread can be a good example.
/// - You need to make an exit point from the DSP pipeline and you need to
///   transmit the resulting data. This is usually done by dedicated
///   communication threads (many times, this is also a library requirement).
///
/// Keep in mind that
/// PipeRT pipelines can have multiple inputs and outputs, so it is not a
/// problem to have multiple exit points and feed back data which once already
/// left the pipeline if the timestamps are kept correct.
/// Also note that one thread can simultaneously poll multiple PolledChannel
/// objects if it is needed (can be useful for your main thread).
///
/// \tparam T Same as in the case of `Channel<T>`.
template <class T>
class PolledChannel : public Channel<T> {
 public:
  /// A Channel can only be created directly from the implementation.
  /// Call Scheduler::CreatePolledChannel() to create one.
  PolledChannel(ChannelImpl* impl);

  PolledChannel(PolledChannel&&) = default;
  PolledChannel& operator=(PolledChannel&&) = default;

  /// Get the next Packet for processing according to the scheduling rules.
  ///
  /// This function returns immediately.
  /// It gets the next Packet out of the Channel queue and returns it.
  /// It can return an empty stub in case no Packets are available.
  /// \return PacketToProcess _stub_  pointing to the next Packet to be
  ///         processed or an empty object (see PacketStub::IsEmpty()) if
  ///         there is no queued Packet available in the buffer.
  PacketToProcess<T> Poll();
};

template <class T>
PolledChannel<T>::PolledChannel(ChannelImpl* impl) : Channel<T>(impl) {}

template <class T>
PacketToProcess<T> PolledChannel<T>::Poll() {
  Packet<T>* new_packet = reinterpret_cast<Packet<T>*>(this->GetNext());
  return PacketToProcess<T>(new_packet, this);
}

}  // namespace pipert

#endif  //_POLLEDCHANNEL_H_