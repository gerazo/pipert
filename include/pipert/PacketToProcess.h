#ifndef _PACKETTOPROCESS_H_
#define _PACKETTOPROCESS_H_

#include "pipert/PacketStub.h"

namespace pipert {

template <class T>
class PacketToProcess : public PacketStub<T> {
 public:
  PacketToProcess(Packet<T>* packet, Channel<T>* channel);
  ~PacketToProcess();
  void Release();  ///< Force releasing the packet before the destruction of this stub
};

template <class T>
PacketToProcess<T>::PacketToProcess(Packet<T>* packet, Channel<T>* channel)
  : PacketStub(packet, channel) {
  assert(channel_);
  // can be created empty
}

template <class T>
PacketToProcess<T>::~PacketToProcess() {
  if (!IsEmpty()) channel_->Release(this);
}

template <class T>
void PacketToProcess<T>::Release() {
  assert(!IsEmpty());
  channel_->Release(this);
  packet_ = nullptr;
}

}  // namespace pipert

#endif  //_PACKETTOPROCESS_H_