#ifndef _PACKETSTUB_H_
#define _PACKETSTUB_H_

#include <cassert>
#include <utility>
#include "pipert/Packet.h"
#include "pipert/Channel.h"

namespace pipert {

template <class T>
class Channel;

template <class T>
class PacketStub {
 public:
  PacketStub(const PacketStub&) = delete;
  PacketStub& operator=(const PacketStub&) = delete;
  PacketStub(PacketStub&&);
  PacketStub& operator=(PacketStub&&);

  bool IsEmpty() const;
  Timer::Time timestamp() const;
  const T& data() const;
  T& data();
  Packet<T>* GetPacket();

protected:
  PacketStub(Packet<T>* packet, Channel<T>* channel);

  Channel<T>* channel_;
  Packet<T>* packet_;

 private:
  void move(PacketStub&&);
};

template <class T>
PacketStub<T>::PacketStub(PacketStub&& o) {
  move(std::move(o));
}

template <class T>
PacketStub<T>& PacketStub<T>::operator=(PacketStub&& o) {
  move(std::move(o));
  return *this;
}

template <class T>
bool PacketStub<T>::IsEmpty() const {
  return packet_ == nullptr;
}

template <class T>
Timer::Time PacketStub<T>::timestamp() const {
  assert(!IsEmpty());
  return packet_->timestamp();
}

template <class T>
const T& PacketStub<T>::data() const {
  assert(!IsEmpty());
  return packet_->data();
}

template <class T>
T& PacketStub<T>::data() {
  assert(!IsEmpty());
  return packet_->data();
}

template <class T>
Packet<T>* PacketStub<T>::GetPacket() {
  return packet_;
}

template <class T>
PacketStub<T>::PacketStub(Packet<T>* packet, Channel<T>* channel)
  : channel_(channel), packet_(packet) {}

template <class T>
void PacketStub<T>::move(PacketStub&& o) {
  channel_ = o.channel_;
  packet_ = o.packet_;
  o.packet_ = nullptr;
}

}  // namespace pipert

#endif  //_PACKETSTUB_H_