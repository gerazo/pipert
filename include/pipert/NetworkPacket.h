#ifndef _NETWORKPACKET_H_
#define _NETWORKPACKET_H_

#include <type_traits>

#include "pipert/Packet.h"
#include "pipert/Timer.h"

namespace pipert {

template <class T>
class NetworkPacket{
 public:
  NetworkPacket() = default;
  NetworkPacket(Timer::Time timestamp, T& data);
  ~NetworkPacket() = default;

  NetworkPacket(const NetworkPacket<T>&) = default;
  NetworkPacket& operator=(const NetworkPacket&) = default;

  const T& data() const { return data_; }
  T& data() {return data_; }
  Timer::Time timestamp() const { return timestamp_; }

 private:
  Timer::Time timestamp_;
  T data_;
};

template <class T>
NetworkPacket<T>::NetworkPacket(Timer::Time timestamp, T& data)
    : timestamp_(timestamp), data_(data) {
  static_assert(std::is_trivially_copyable<T>::value,
                  "Network protocol requires trivially copyable type");
}

template <class T>
NetworkPacket<T> ConvertToNetworkPacket(Packet<T>& packet) {
  return NetworkPacket<T>(packet.timestamp(), packet.data());
}

}  // namespace pipert

#endif  //_NETWORKPACKET_H_
