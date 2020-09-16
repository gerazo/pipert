#ifndef _CHANNELBASE_H_
#define _CHANNELBASE_H_

#include "pipert/PacketBase.h"

namespace pipert {

class ChannelImpl;

class ChannelBase {
 public:
  using InternalCallback = void (*)(ChannelBase*, PacketBase*);

  ChannelBase(const ChannelBase&) = delete;
  ChannelBase& operator=(const ChannelBase&) = delete;
  ChannelBase(ChannelBase&&);
  ChannelBase& operator=(ChannelBase&&);

  int GetCapacity() const;  ///< Number of packets the channel buffer has space for
  int GetPacketSize() const;
  const char* GetName() const;

 protected:
  ChannelBase(ChannelImpl* impl);
  ~ChannelBase();

  PacketBase* AcquireBase(const char* client_name);
  void PushBase(PacketBase* packet);
  PacketBase* GetNext();
  void ReleaseBase(PacketBase* packet);

 private:
  void move(ChannelBase&&);

  ChannelImpl* impl_;
};

}  // namespace pipert

#endif  //_CHANNELBASE_H_