#ifndef _PACKET_IMP_H_
#define _PACKET_IMP_H_

#include "Timer.h"

namespace pipert {

using Time = uint64_t;

template <class T>
class PacketImp {
 public:
  PacketImp(const T& data);
  PacketImp(const T& data, Time created_at);
  template <class T2>
  PacketImp(const T& data, const PacketImp<T2>* packet);

  ~PacketImp() { /*std::cout << "PacketImp destroyed: " << created_at_ <<
                    std::endl;*/
  }

  const T* GetData() const;
  T* GetData();

  Time GetCreatedTime() const;
  Time GetCurrentTime() const;

  void SetCreatedTime(Time created_at);

 private:
  Time created_at_;  // timestamp when package was first created
  Time current_;     // timestamp which referesh when package processed
  T raw_data_;       // the data stored in the package
};

template <class T>
PacketImp<T>::PacketImp(const T& data, Time created_at)
    : raw_data_(data), created_at_(created_at), current_(created_at) {}

template <class T>
PacketImp<T>::PacketImp(const T& data)
    : raw_data_(data),
      created_at_(Timer::time()),
      current_(created_at_)  // should moving data not copy
{}

template <class T>
template <class T2>
PacketImp<T>::PacketImp(const T& data, const PacketImp<T2>* packetImp)
    : raw_data_(data),
      created_at_(packetImp->GetCreatedTime()),
      current_(Timer::time())  // should moving data not copy
{}

template <class T>
const T* PacketImp<T>::GetData() const {
  return &raw_data_;
}

template <class T>
T* PacketImp<T>::GetData() {
  return &raw_data_;
}

template <class T>
Time PacketImp<T>::GetCreatedTime() const {
  return created_at_;
}

template <class T>
Time PacketImp<T>::GetCurrentTime() const {
  return current_;
}

template <class T>
void PacketImp<T>::SetCreatedTime(Time created_at) {
  this->created_at_ = created_at;
  current_ = created_at;
}

}  // namespace pipert

#endif  //_PACKET_IMP_H_