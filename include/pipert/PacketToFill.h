#ifndef _PACKETTOFILL_H_
#define _PACKETTOFILL_H_

#include "pipert/PacketStub.h"

namespace pipert {

template <class T>
class PacketToFill : public PacketStub<T> {
 public:
  PacketToFill(Packet<T>* packet, Channel<T>* channel);
  ~PacketToFill();
  void Push();  ///< Force submitting packet before the destruction of this stub
};

template <class T>
PacketToFill<T>::PacketToFill(Packet<T>* packet, Channel<T>* channel)
  : PacketStub(packet, channel) {
  assert(channel_);
  assert(packet_);  // always created initialized
}

template <class T>
PacketToFill<T>::~PacketToFill() {
  if (!IsEmpty()) channel_->Push(this);
}

template <class T>
void PacketToFill<T>::Push() {
  assert(!IsEmpty());
  channel_->Push(this);
  packet_ = nullptr;
}

}  // namespace pipert

#endif  //_PACKETTOFILL_H_