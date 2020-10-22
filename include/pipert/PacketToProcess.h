#ifndef _PACKETTOPROCESS_H_
#define _PACKETTOPROCESS_H_

#include "pipert/PacketStub.h"

namespace pipert {

/// A RAII object representing a Packet which is sent for processing.
///
/// You can access the connected Packet being in the buffer of a Channel
/// and access the data in it to be processed.
/// The connected Packet is automatically discarded in the connected Channel on
/// the destruction of this object.
///
/// See PacketStub for details.
///
/// \tparam T Follows the template paramter of Packet.
///           See template paramter T of Packet for details.
template <class T>
class PacketToProcess : public PacketStub<T> {
 public:
  PacketToProcess(Packet<T>* packet, Channel<T>* channel);
  ~PacketToProcess();
  PacketToProcess(PacketToProcess&&) = default;
  PacketToProcess& operator=(PacketToProcess&&) = default;

  /// Force releasing the packet before the destruction of this stub.
  ///
  /// You can do an early freeing of the Packet by using this.
  /// It is useful as the data is immediately freed for
  /// reusing on releasing.
  /// As a result, this object will become empty, see PacketStub::IsEmpty().
  /// Also see Channel::Release().
  void Release();

  friend void Channel<T>::Release(PacketToProcess<T>* processed_packet);
};

template <class T>
PacketToProcess<T>::PacketToProcess(Packet<T>* packet, Channel<T>* channel)
  : PacketStub<T>(packet, channel) {
  assert(this->channel_);
  // can be created empty
}

template <class T>
PacketToProcess<T>::~PacketToProcess() {
  if (!this->IsEmpty()) Release();
}

template <class T>
void PacketToProcess<T>::Release() {
  assert(!this->IsEmpty());
  this->channel_->Release(this);
  assert(this->IsEmpty());
}

}  // namespace pipert

#endif  //_PACKETTOPROCESS_H_