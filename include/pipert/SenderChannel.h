#ifndef _SENDERCHANNEL_H_
#define _SENDERCHANNEL_H_

#include "pipert/NetworkPacket.h"
#include "pipert/ScheduledChannel.h"

#include <functional>

namespace pipert {

template <class T>
class SenderChannel : public ScheduledChannel<T> {
 public:
  SenderChannel(ChannelImpl* impl, UDPConnection* connection);

  SenderChannel(SenderChannel&&) = default;
  SenderChannel& operator=(SenderChannel&&) = default;

  void SendPacket(PacketToProcess<T> packet);

 private:
  UDPConnection* connection_;
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
