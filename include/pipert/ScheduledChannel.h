#ifndef _SCHEDULEDCHANNEL_H_
#define _SCHEDULEDCHANNEL_H_

#include <functional>
#include "pipert/Channel.h"

namespace pipert {

template <class T>
class ScheduledChannel : public Channel<T> {
 public:
  using Callback = std::function<void(PacketToProcess<T>)>;

  ScheduledChannel(char* name, int capacity, void* this_mutex, Callback processing_fun);

 private:
  Callback processing_fun_;
};

template <class T>
ScheduledChannel<T>::ScheduledChannel(char* name, int capacity, void* this_mutex, Callback processing_fun)
  : Channel(name, capacity, this_mutex), processing_fun_(processing_fun) {}

}  // namespace pipert

#endif  //_SCHEDULEDCHANNEL_H_