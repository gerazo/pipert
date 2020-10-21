#ifndef _CHANNELBASE_H_
#define _CHANNELBASE_H_

#include "pipert/PacketBase.h"

namespace pipert {

class ChannelImpl;

/// A template free base class of Channel to hold general parameters.
///
/// This class is the base of Channel. Here, all operations are already
/// done without templates, so we can easily call the implementation.
/// It also holds general settings and paramters of a Channel.
class ChannelBase {
 public:
  /// (_Internal implementation_) C-style callback to be able to
  /// call a ScheduledChannel::Callback from inside the implementation.
  using InternalCallback = void (*)(ChannelBase*, PacketBase*);

  ChannelBase(const ChannelBase&) = delete;
  ChannelBase& operator=(const ChannelBase&) = delete;
  ChannelBase(ChannelBase&&);
  ChannelBase& operator=(ChannelBase&&);

  /// Get how many Packet objects the buffer of this Channel has.
  /// \return Number of Packet objects the Channel buffer has space for.
  int GetCapacity() const;

  /// Get the size of Packet objects which the buffer has.
  /// \return Size of Packet object having T template parameter.
  int GetPacketSize() const;

  /// Get the name of this Channel.
  ///
  /// It is practical to have a unique name for every Channel in case of a
  /// _Node_ with multiple Channels.
  /// In case of one Channel for a _Node_ you can use the name of the _Node_.
  /// \return Preferably unique name which can be used for
  ///         logging and monitoring.
  const char* GetName() const;

 protected:
  ChannelBase(ChannelImpl* impl);
  ~ChannelBase();

  /// Call into the implementation of asking for a packet which can be filled.
  /// \param client_name Name of _Node_ where data is coming from
  ///                    (name of filler object).
  /// \return Pointer to Packet or `nullptr` on no available packets.
  PacketBase* AcquireBase(const char* client_name);

  /// Call into the implementation of enqueing Packet in the Channel
  /// for processing.
  /// \param packet Pointer to Packet.
  void PushBase(PacketBase* packet);

  /// Call into the implementation of getting the next scheduled job in buffer.
  /// \return Pointer to Packet or `nullptr` on no available packets.
  PacketBase* GetNext();

  /// Call into the implementation of releasing a Packet after processing.
  /// \param packet Pointer to Packet.
  void ReleaseBase(PacketBase* packet);

 private:
  void move(ChannelBase&&);

  /// Pointer to implementation which is already a template free code.
  ChannelImpl* impl_;
};

}  // namespace pipert

#endif  //_CHANNELBASE_H_