#ifndef _RECEIVERCHANNEL_H_
#define _RECEIVERCHANNEL_H_

#include "pipert/Channel.h"

namespace pipert {

template <class T>
class ReceiverChannel : public Channel<T> {
 public:
  ReceiverChannel(ChannelImpl* impl, UDPConnection* connection);

  ReceiverChannel(ReceiverChannel&&) = default;
  ReceiverChannel& operator=(ReceiverChannel&&) = default;

 private:
  UDPConnection* connection_;
};

template <class T>
ReceiverChannel<T>::ReceiverChannel(ChannelImpl* impl,
                                    UDPConnection* connection)
    : Channel<T>(impl), connection_(connection) {}

}  // namespace pipert

#endif  //_RECEIVERCHANNEL_H_
