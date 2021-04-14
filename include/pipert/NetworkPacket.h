#ifndef _NETWORKPACKET_H_
#define _NETWORKPACKET_H_

#include <type_traits>

#include "pipert/Packet.h"
#include "pipert/Timer.h"

namespace pipert {

template <class T>
class NetworkPacket : public Packet<T> {
 public:
  NetworkPacket() = default;
  NetworkPacket(Timer::Time timestamp, T& data);
  ~NetworkPacket() = default;

  NetworkPacket(const NetworkPacket<T>&) = default;
  NetworkPacket& operator=(const NetworkPacket&) = default;
};

template <class T>
NetworkPacket<T>::NetworkPacket(Timer::Time timestamp, T& data)
    : Packet<T>(timestamp, data) {
  static_assert(std::is_trivially_copyable<T>::value,
                  "Network protocol requires trivially copyable type");
}

}  // namespace pipert

#endif  //_NETWORKPACKET_H_
