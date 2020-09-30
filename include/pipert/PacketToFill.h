#ifndef _PACKETTOFILL_H_
#define _PACKETTOFILL_H_

#include "pipert/PacketStub.h"

namespace pipert {

template <class T>
class PacketToFill : public PacketStub<T> {
 public:
  PacketToFill(Packet<T>* packet, Channel<T>* channel);
  ~PacketToFill();
  PacketToFill(PacketToFill&&) = default;
  PacketToFill& operator=(PacketToFill&&) = default;
  void Push();  ///< Force submitting packet before the destruction of this stub

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