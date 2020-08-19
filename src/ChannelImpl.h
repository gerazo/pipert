#ifndef _CHANNELIMPL_H_
#define _CHANNELIMPL_H_

#include "pipert/ChannelBase.h"

namespace pipert {

class ChannelImpl {
 public:
  ChannelImpl(char* name, int capacity, int packet_size, void* this_mutex, ChannelBase::InternalCallback callback);
  ~ChannelImpl();

  int GetCapacity() const { return capacity_; }
  int GetPacketSize() const { return packet_size_; }
  const char* GetName() const { return name_; }

 private:
  void* this_mutex_;
  ChannelBase::InternalCallback callback_;
  char* name_;
  int capacity_;
  int packet_size_;
};

}  // namespace pipert

#endif  //_CHANNELIMPL_H_