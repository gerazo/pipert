#ifndef _CHANNELBASE_H_
#define _CHANNELBASE_H_

namespace pipert {

class ChannelImpl;

class ChannelBase {
 public:
  ChannelBase(const ChannelBase&) = delete;
  ChannelBase& operator=(const ChannelBase&) = delete;

  int GetCapacity() const;  ///< Number of packets the channel buffer has space for
  int GetPacketSize() const;
  const char* GetName() const;

 protected:
  ChannelBase(char* name, int capacity, int packet_size, void* this_mutex);
  ~ChannelBase();

 private:
  ChannelImpl* impl_;
};

}  // namespace pipert

#endif  //_CHANNELBASE_H_