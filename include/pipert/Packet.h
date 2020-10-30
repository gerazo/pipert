#ifndef _PACKET_H_
#define _PACKET_H_

#include <utility>
#include "pipert/PacketBase.h"

namespace pipert {

/// Represents data which is transmitted between nodes for processing.
///
/// Packets contain data which can be processed in one run by a _node_.
/// Packets are sent to a processing _node_ by being placed onto the Channel
/// which was created by that receiving _node_.
/// (_Nodes_ are user written objects which do arbitrary operations on data.)
///
/// Packets are always allocated in the buffer of a Channel at setup time.
/// No memory allocations are done during runtime, packets are always reused
/// from the buffer, so they always stay in place.
/// Packets normally do not exist alone without their containing Channels.
///
/// Packets are going through the following phases during runtime:
/// - _Free_: Packet is waiting to be used by anyone who has access to its
///           Channel.
/// - _Being Filled_: After Packet acquisition, the caller fills the Packet
///                   with data.
/// - _Queued_: The filled Packet is put back onto the queue of a Channel where
///             it is waiting to be processed.
/// - _Being Processed_: Packet was sent to the processing code by the
///                      Scheduler which is currently using it.
///
/// See also PacketBase::timestamp()
///
/// \tparam T Type of user data which this packet will contain.
///           This type will be constructed and destructed in place.
///           No copying will be done during the lifetime of the Packet.
///           All data should be placed into this object (POD).
///           Doing extra memory allocations on construction
///           (and pointing to them) will slow down the system
///           or will have even worse consequences, threading issues.
template <class T>
class Packet : public PacketBase {
 public:
  /// Constructs a Packet by initializing the contained datatype in place.
  /// \param timestamp The exact time when this data or the
  ///                  observed phenomenon was recorded.
  ///                  See PacketBase::timestamp() for details.
  /// \param args Constructor parameters of user type T which will be
  ///             constructed in place.
  template <class... Args>
  Packet(Timer::Time timestamp, Args&&... args);
  ~Packet() {}

  /// Read-only access to the contained data.
  /// \return Read-only reference to the data.
  const T& data() const;

  /// Read-write access to the contained data.
  /// \return Reference to the contained data.
  T& data();

 private:
  T data_;  ///< Data stored in this package
};

template <class T>
template <class... Args>
Packet<T>::Packet(Timer::Time timestamp, Args&&... args)
    : PacketBase(timestamp), data_(std::forward<Args>(args)...) {}

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