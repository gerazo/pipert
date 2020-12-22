#ifndef _CHANNELBASE_H_
#define _CHANNELBASE_H_

#include "pipert/LogEventBase.h"
#include "pipert/PacketBase.h"

namespace pipert {

class ChannelImpl;

/// A template free base class of Channel to hold its settings and general
/// parameters.
///
/// All operations are done without templates, so the implementation can be
/// called easily.
class ChannelBase {
 public:
  /// (_Internal implementation_) C-style callback to be able to
  /// call a ScheduledChannel::Callback from inside the implementation.
  using InternalCallback = void (*)(ChannelBase*, PacketBase*);

  ChannelBase(const ChannelBase&) = delete;
  ChannelBase& operator=(const ChannelBase&) = delete;
  ChannelBase(ChannelBase&&);
  ChannelBase& operator=(ChannelBase&&);

  /// Get the maximum amount of packets the buffer of this Channel can hold.
  /// \return Number of Packet objects the Channel buffer has space for.
  int GetCapacity() const;

  /// Get the size of Packet objects held by the buffer, in bytes.
  /// \return Size of a Packet object having T as its template parameter.
  int GetPacketSize() const;

  /// Get the name of this Channel.
  ///
  /// It is practical to have a unique name for every Channel in the case of a
  /// _Node_ with multiple Channels.
  /// If you have only one Channel for a _Node_ you can simply use the name of
  //// the _Node_.
  /// \return Preferably unique name which can be used for
  ///         logging and monitoring.
  const char* GetName() const;

  /// Returns how many, currently usable packets are present in the Channel.
  int GetFreeBufferLength() const;

  /// Returns how many packets are standing in the queue of the Channel.
  int GetQueuedBufferLength() const;

  /// Logs an event connected to this Channel.
  /// \param log_event The user created LogEvent object.
  /// \note This function is thread-safe.
  void Log(LogEventBase log_event);

 protected:
  /// Construct a ChannelBase using an implementation that must be provided.
  /// Calls to ChannelBase will be delegated to this implementation.
  ///
  /// \param impl The obligatorily provided hidden implementation of Channel.
  ChannelBase(ChannelImpl* impl);
  ~ChannelBase();

  /// Call into the implementation to ask for a memory location for a packet
  /// that is available for filling.
  /// \param client_name Name of the _Node_ where data is coming from
  ///                    (name of the filler object).
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