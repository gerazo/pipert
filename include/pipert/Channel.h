#ifndef _CHANNEL_H_
#define _CHANNEL_H_

#include "pipert/ChannelBase.h"
#include "pipert/Packet.h"
#include "pipert/PacketToFill.h"
#include "pipert/PacketToProcess.h"

namespace pipert {

template <class T>
class Channel : public ChannelBase {
 public:
  template <class... Args>
  PacketToFill<T> Acquire(const char* client_name, Timer::Time timestamp, Args&&... args);  ///< Always returns, but sometimes old packets are dropped
  void Push(PacketToFill<T>* filled_packet);
  void Release(PacketToProcess<T>* processed_packet);

 protected:
  Channel(char* name, int capacity, void* this_mutex);
};

template <class T>
Channel<T>::Channel(char* name, int capacity, void* this_mutex)
  : ChannelBase(name, capacity, sizeof(Packet<T>), this_mutex) {}

}  // namespace pipert

#endif  //_CHANNEL_H_