#ifndef _SENDERCHANNEL_H_
#define _SENDERCHANNEL_H_

#include "pipert/Channel.h"
#include "pipert/NetworkPacket.h"

#include <functional>

namespace pipert {

template <class T>
class SenderChannel : public Channel<T> {
 public:
  SenderChannel(ChannelImpl* impl, UDPConnection* connection);

  SenderChannel(SenderChannel&&) = default;
  SenderChannel& operator=(SenderChannel&&) = default;

  static void SenderCallback(ChannelBase* this_channel, PacketBase* packet);

  void SendPacket(PacketToProcess<T> packet);

 private:
  UDPConnection* connection_;
};

template <class T>
SenderChannel<T>::SenderChannel(ChannelImpl* impl,
                                UDPConnection* connection)
    : Channel<T>(impl), connection_(connection) {}

template <class T>
void SenderChannel<T>::SenderCallback(ChannelBase* this_channel,
                                      PacketBase* packet) {
  assert(this_channel);
  SenderChannel<T>* my_this =
      reinterpret_cast<SenderChannel<T>*>(this_channel);
  assert(my_this);
  std::function<void(PacketToProcess<T>)> processing_fun =
      std::bind(&SenderChannel<T>::SendPacket, my_this, std::placeholders::_1);
  processing_fun(
      PacketToProcess<T>(reinterpret_cast<Packet<T>*>(packet), my_this));
}

template <class T>
void SenderChannel<T>::SendPacket(PacketToProcess<T> packet) {
  NetworkPacket<T> network_packet(packet.timestamp(), packet.data());
  connection_->Send(&network_packet, sizeof(network_packet));
}

}  // namespace pipert

#endif  //_SENDERCHANNEL_H_
