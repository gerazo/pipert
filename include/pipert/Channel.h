#ifndef _CHANNEL_H_
#define _CHANNEL_H_

#include <cassert>
#include <utility>
#include "pipert/ChannelBase.h"
#include "pipert/Packet.h"
#include "pipert/PacketToFill.h"
#include "pipert/PacketToProcess.h"

namespace pipert {

template <class T>
class Channel : public ChannelBase {
 public:
  template <class... Args>
  PacketToFill<T> Acquire(const char* client_name, Timer::Time timestamp, Args&&... args);  ///< Always returns, but sometimes old packets are dropped
  void Push(PacketToFill<T>* filled_packet);
  void Release(PacketToProcess<T>* processed_packet);

 protected:
  Channel(char* name, int capacity, void* mutex_state, InternalCallback callback);
};

template <class T>
Channel<T>::Channel(char* name, int capacity, void* mutex_state, InternalCallback callback)
  : ChannelBase(name, capacity, sizeof(Packet<T>), mutex_state, callback) {}

template <class T> template <class... Args>
PacketToFill<T> Channel<T>::Acquire(const char* client_name, Timer::Time timestamp, Args&&... args) {
  Packet<T>* new_packet = reinterpret_cast<Packet<T>*>(Acquire(client_name));
  assert(new_packet);
  new(new_packet) Packet<T>(timestamp, std::forward<Args>(args)...);
  return PacketToFill<T>(new_packet, this);
}

template <class T>
void Channel<T>::Push(PacketToFill<T>* filled_packet) {
  assert(!filled_packet->IsEmpty());
  Push(filled_packet->GetPacket());
}

template <class T>
void Channel<T>::Release(PacketToProcess<T>* processed_packet) {
  assert(!processed_packet->IsEmpty());
  Packet<T>* packet = processed_packet->GetPacket();
  packet->~Packet();
  Release(packet);
}

}  // namespace pipert

#endif  //_CHANNEL_H_