#ifndef _SENDERCHANNEL_H_
#define _SENDERCHANNEL_H_

#include "pipert/NetworkPacket.h"
#include "pipert/ScheduledChannel.h"

#include <functional>

namespace pipert {

/// A Channel whose packets are automatically scheduled to be sent to
/// the given remote computer.
///
/// SenderChannel is a special type of SchedulerChannel, where the
/// Packets from the channel's buffer are automatically scheduled.
/// The function to be called when processing data is always the
/// SendPacket() function.
///
/// \tparam T Same as in the case of `Channel<T>`.
template <class T>
class SenderChannel : public ScheduledChannel<T> {
 public:
  /// A Channel can only be created directly from the implementation.
  /// Call Scheduler::CreateSenderChannel() to create one.
  SenderChannel(ChannelImpl* impl, UDPConnection* connection);

  SenderChannel(SenderChannel&&) = default;
  SenderChannel& operator=(SenderChannel&&) = default;

  /// Converts Packets to NetworkPackets and sends them using
  /// the given UDPConnection object.
  /// \param packet Packet to be processed from the channel's buffer.
  void SendPacket(PacketToProcess<T> packet);

 private:
  UDPConnection* connection_;  ///< Network properties of the receiver side.
};

template <class T>
SenderChannel<T>::SenderChannel(ChannelImpl* impl,
                                UDPConnection* connection)
    : ScheduledChannel<T>(impl,
      std::bind(&SenderChannel<T>::SendPacket, this, std::placeholders::_1)),
      connection_(connection) {}

template <class T>
void SenderChannel<T>::SendPacket(PacketToProcess<T> packet) {
  NetworkPacket<T> network_packet(packet.timestamp(), packet.data());
  connection_->Send(&network_packet, sizeof(network_packet));
}

}  // namespace pipert

#endif  //_SENDERCHANNEL_H_
