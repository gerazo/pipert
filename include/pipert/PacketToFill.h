#ifndef _PACKETTOFILL_H_
#define _PACKETTOFILL_H_

#include "pipert/PacketStub.h"

namespace pipert {

/// A RAII object representing a Packet which is about to be filled with data.
///
/// You can access the connected Packet being in the buffer of a Channel
/// and fill it with data.
/// The connected Packet is automatically queued in the connected Channel on
/// the destruction of this object.
///
/// See PacketStub for details.
///
/// \tparam T See template paramter T of Packet
template <class T>
class PacketToFill : public PacketStub<T> {
 public:
  PacketToFill(Packet<T>* packet, Channel<T>* channel);
  ~PacketToFill();
  PacketToFill(PacketToFill&&) = default;
  PacketToFill& operator=(PacketToFill&&) = default;

  /// Force submission of packet before the destruction of this stub.
  ///
  /// You can do an early submission by using this.
  /// It is useful as the data is immediately queded for
  /// processing on pushing.
  /// As a result, this object will become empty, see PacketStub::IsEmpty()
  void Push();

  friend void Channel<T>::Push(PacketToFill<T>* filled_packet);
};

template <class T>
PacketToFill<T>::PacketToFill(Packet<T>* packet, Channel<T>* channel)
  : PacketStub<T>(packet, channel) {
  assert(this->channel_);
  // can be created empty
}

template <class T>
PacketToFill<T>::~PacketToFill() {
  if (!this->IsEmpty()) Push();
}

template <class T>
void PacketToFill<T>::Push() {
  assert(!this->IsEmpty());
  this->channel_->Push(this);
  assert(this->IsEmpty());
}

}  // namespace pipert

#endif  //_PACKETTOFILL_H_