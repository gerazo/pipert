#ifndef _SENDERCHANNEL_H_
#define _SENDERCHANNEL_H_

#include "pipert/Channel.h"

namespace pipert {

class UDPConnection;

template <class T>
class SenderChannel : public Channel<T> {
 public:
  SenderChannel(ChannelImpl* impl, const UDPConnection& connection);

  SenderChannel(SenderChannel&&) = default;
  SenderChannel& operator=(SenderChannel&&) = default;

 private:
  UDPConnection connection_;
};

template <class T>
SenderChannel<T>::SenderChannel(ChannelImpl* impl,
                                const UDPConnection& connection)
    : Channel<T>(impl), connection_(connection) {}

}  // namespace pipert

#endif  //_SENDERCHANNEL_H_
