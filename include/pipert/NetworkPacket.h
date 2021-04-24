#ifndef _NETWORKPACKET_H_
#define _NETWORKPACKET_H_

#include <type_traits>

#include "pipert/Packet.h"
#include "pipert/Timer.h"

namespace pipert {

/// Represents data which is transmitted between computers for processing.
///
/// NetworkPacket is a special type of Packet, which is used for pipeline
/// communication between separated computers on the network.
/// See Packet class for details.
///
/// \tparam T Type of user data which this packet will contain.
///           Type must be Trivially Copyable to be appropriate for
///           network communication.
template <class T>
class NetworkPacket : public Packet<T> {
 public:
  /// Default constructor for NetworkPacket.
  NetworkPacket() = default;
  /// Constructs a NetworkPacket with the given timestamp and data.
  /// Type of data is required to be Trivially Copyable type and it
  /// is being checked during compile time.
  /// \param timestamp The exact time when this data or the
  ///                  observed phenomenon was recorded.
  ///                  See PacketBase::timestamp() for details.
  /// \param data Predefined data with type T. It comes from
  ///             Packet::data().
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
