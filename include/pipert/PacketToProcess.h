#ifndef _PACKETTOPROCESS_H_
#define _PACKETTOPROCESS_H_

#include "pipert/PacketStub.h"

namespace pipert {

template <class T>
class PacketToProcess : public PacketStub<T> {
 public:
  PacketToProcess(Packet<T>* packet, Channel<T>* channel);
  ~PacketToProcess();
  PacketToProcess(PacketToProcess&&) = default;
  PacketToProcess& operator=(PacketToProcess&&) = default;
  void Release();  ///< Force releasing the packet before the destruction of this stub
};

template <class T>
PacketToProcess<T>::PacketToProcess(Packet<T>* packet, Channel<T>* channel)
  : PacketStub<T>(packet, channel) {
  assert(this->channel_);
  // can be created empty
}

template <class T>
PacketToProcess<T>::~PacketToProcess() {
  if (!this->IsEmpty()) this->channel_->Release(this);
}

template <class T>
void PacketToProcess<T>::Release() {
  assert(!this->IsEmpty());
  this->channel_->Release(this);
  this->packet_ = nullptr;
}

}  // namespace pipert

#endif  //_PACKETTOPROCESS_H_