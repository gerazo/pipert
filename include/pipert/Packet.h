#ifndef _PACKET_H_
#define _PACKET_H_

#include <utility>
#include "pipert/Timer.h"

namespace pipert {

template <class T>
class Packet {
 public:
  template <class... Args>
  Packet(Timer::Time created_at, Args&&... args);
  Packet(const Packet&) = delete;
  Packet& operator=(const Packet&) = delete;
  ~Packet() {}

  Timer::Time GetCreatedTime() const;
  const T& data() const;
  T& data();

 private:
  Timer::Time created_at_;  // timestamp when package was first created
  T data_;  // the data stored in the package
};

template <class T> template <class... Args>
Packet<T>::Packet(Timer::Time created_at, Args&&... args)
  : created_at_(created_at), data_(std::forward<Args>(args)...) {}

template <class T>
Timer::Time Packet<T>::GetCreatedTime() const {
  return created_at_;
}

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