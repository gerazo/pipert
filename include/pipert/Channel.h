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
  PacketToFill<T> Acquire(const char* client_name, Timer::Time timestamp,
                          Args&&... args);  ///< Returns empty object when all packets are used
  void Push(PacketToFill<T>* filled_packet);
  void Release(PacketToProcess<T>* processed_packet);

 protected:
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