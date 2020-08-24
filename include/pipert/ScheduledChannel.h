#ifndef _SCHEDULEDCHANNEL_H_
#define _SCHEDULEDCHANNEL_H_

#include <functional>
#include "pipert/Channel.h"

namespace pipert {

template <class T>
class ScheduledChannel : public Channel<T> {
 public:
  using Callback = std::function<void(PacketToProcess<T>)>;

  ScheduledChannel(ChannelImpl* impl, Callback processing_fun);

  static void CallbackTranslator(ChannelBase* this_channel, PacketBase* packet);

 private:
  Callback processing_fun_;
};

template <class T>
ScheduledChannel<T>::ScheduledChannel(ChannelImpl* impl, Callback processing_fun)
  : Channel<T>(impl), processing_fun_(processing_fun) {}

template <class T>
void ScheduledChannel<T>::CallbackTranslator(ChannelBase* this_channel,
                                             PacketBase* packet) {
  ScheduledChannel<T>* my_this =
    reinterpret_cast<ScheduledChannel<T>*>(this_channel);
  my_this->processing_fun_(PacketToProcess<T>(
      reinterpret_cast<Packet<T>*>(packet), my_this));
}

}  // namespace pipert

#endif  //_SCHEDULEDCHANNEL_H_