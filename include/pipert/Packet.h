#ifndef _PACKET_H_
#define _PACKET_H_

#include <utility>
#include "pipert/PacketBase.h"

namespace pipert {

template <class T>
class Packet : public PacketBase {
 public:
  template <class... Args>
  Packet(Timer::Time created_at, Args&&... args);
  ~Packet() {}

  const T& data() const;
  T& data();

 private:
  T data_;  // the data stored in the package
};

template <class T> template <class... Args>
Packet<T>::Packet(Timer::Time created_at, Args&&... args)
  : PacketBase(created_at), data_(std::forward<Args>(args)...) {}

template <class T>
const T& Packet<T>::data() const {
  return data_;
}

template <class T>
T& Packet<T>::data() {
  return data_;
}

}  // namespace pipert

#endif  //_PACKET_H_