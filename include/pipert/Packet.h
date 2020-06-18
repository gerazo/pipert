#ifndef _PACKET_HPP_
#define _PACKET_HPP_

#include <memory>
#include "PacketImp.h"

namespace pipert {

template <class T>
class Packet {
 public:
  using Time = uint64_t;

  Packet(const T& data, Time created_at);
  Packet(const T& data);
  template <class T2>
  Packet(const T& data, const Packet<T2>& packet);

  ~Packet() { /*std::cout << "Packet destroyed: " << getCreatedTime() <<
                 std::endl;*/
  }

  Packet<T> Clone() const;

  T GetData() const;
  const T* GetDataConstPtr() const;
  T* GetDataPtr();
  Time GetCreatedTime() const;
  Time GetCurrentTime() const;

  void SetCreatedTime(Time created_at);

  const PacketImp<T>* GetImp() const;

 private:
  std::shared_ptr<PacketImp<T> > imp_;

  Packet(PacketImp<T>* imp);
};

template <class T>
Packet<T>::Packet(const T& data, Time created_at)
    : imp_(new PacketImp<T>(data, created_at)) {}

template <class T>
Packet<T>::Packet(const T& data) : imp_(new PacketImp<T>(data)) {}

template <class T>
template <class T2>
Packet<T>::Packet(const T& data, const Packet<T2>& packet)
    : imp_(new PacketImp<T>(data, packet.GetImp())) {}

template <class T>
Packet<T>::Packet(PacketImp<T>* imp) : imp_(imp_) {}

template <class T>
Packet<T> Packet<T>::Clone() const {
  return Packet<T>(new PacketImp<T>(*this->GetImp()));
}

template <class T>
const T* Packet<T>::GetDataConstPtr() const {
  return imp_->GetData();
}

template <class T>
T Packet<T>::GetData() const {
  return *(imp_->GetData());
}

template <class T>
T* Packet<T>::GetDataPtr() {
  return imp_->GetData();
}

template <class T>
typename Packet<T>::Time Packet<T>::GetCreatedTime() const {
  return imp_->GetCreatedTime();
}

template <class T>
typename Packet<T>::Time Packet<T>::GetCurrentTime() const {
  return imp_->GetCurrentTime();
}

template <class T>
void Packet<T>::SetCreatedTime(Packet<T>::Time created_at) {
  imp_->SetCreatedTime(created_at);
}

template <class T>
const PacketImp<T>* Packet<T>::GetImp() const {
  return imp_.get();
}

}  // namespace pipert

#endif  //_PACKET_HPP_