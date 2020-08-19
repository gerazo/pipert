#ifndef _CHANNELIMPL_H_
#define _CHANNELIMPL_H_

#include <cstdint>
#include <vector>
#include "pipert/ChannelBase.h"
#include "AdaptiveSpinLock.h"

namespace pipert {

class ChannelImpl {
 public:
  ChannelImpl(char* name, int capacity, int packet_size, void* mutex_state, ChannelBase::InternalCallback callback);
  ~ChannelImpl();
  ChannelImpl(const ChannelImpl&) = delete;
  ChannelImpl& operator=(const ChannelImpl&) = delete;

  int GetCapacity() const { return capacity_; }
  int GetPacketSize() const { return packet_size_; }
  const char* GetName() const { return name_; }

  PacketBase* Acquire(const char* client_name);
  void Push(PacketBase* packet);
  PacketBase* GetNext();
  void Release(PacketBase* packet);

 private:
  struct PacketOrdering {
    bool operator()(PacketBase* a, PacketBase* b) {
      return a->timestamp() > b->timestamp();
    }
  };

  void* mutex_state_;
  ChannelBase::InternalCallback callback_;
  int8_t* pool_;
  AdaptiveSpinLock free_mutex_;
  std::vector<int> free_packets_;  // already released packets (not in transit as in PacketTo***)
  AdaptiveSpinLock queued_mutex_;
  std::vector<PacketBase*> queued_packets_;  // heap having the oldest timestamp on top
  char* name_;
  int capacity_;
  int packet_size_;
};

}  // namespace pipert

#endif  //_CHANNELIMPL_H_