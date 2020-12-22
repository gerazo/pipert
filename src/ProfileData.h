#ifndef _PROFILE_DATA_H_
#define _PROFILE_DATA_H_

#include "pipert/LogEventBase.h"

#include <cstdint>
#include <unordered_map>

#include "LogAggregate.h"
#include "SenderLogger.h"

namespace pipert {

/// Contains LogAggregate objects, one for every log event type.
///
/// It collects log events and aggregates them in the coresponding LogAggregate
/// objects. It also collects sender thread data.
/// It makes sure that access is happening in a thread-safe way.
/// One ProfileData belongs to one ChannelImpl.
/// It can incrementally serialize all data stored, and also
/// it can incrementally empty those stores which were already serialized.
class ProfileData {
 public:
  /// Time in the Packet timeline when the Packet is filled with data and is
  /// pushed into the Channel buffer.
  static const char kEventPushed[];
  /// Time in the Packet timeline when the Packet is retrieved for processing.
  /// This can either be an `Execute()` or `Poll()` call.
  static const char kEventRetrieved[];
  /// Time spent by a Scheduler thread servicing a Channel callback.
  static const char kEventExecuteTime[];
  /// Time spent between acquiring the Packet and pushing it into the buffer.
  static const char kEventFillTime[];
  /// Time spent between retrieving a Packet and releasing it by the retriever.
  static const char kEventReadTime[];
  /// Packet was dropped because new arrived and the buffer became full.
  static const char kEventDroppedPacket[];

  /// Maximum length of all names (group and event) that will be serialized.
  static const int kMaxNameLength = 27;

  /// Constructs a ProfileData object with the given name.
  /// \param data_group_name The name of this data group.
  ///                        Practically this will be the name of the Channel.
  ProfileData(const char* data_group_name);

  ProfileData(const ProfileData&) = delete;
  ProfileData& operator=(const ProfileData&) = delete;

  /// Get the name of the data group which this object represents.
  /// \return Pointer to the name string.
  const char* GetName() const;

  /// Logs an event which is then aggregated into the internal data structures.
  /// \param log_event The base class of the user created LogEvent object.
  /// \note This function is thread-safe.
  void Log(LogEventBase log_event);

  /// Incrementally serializes the contents of this object into multiple
  /// buffers.
  ///
  /// One call fills up one buffer that can be sent through the network.
  /// The return value tells whether more calls are needed to dump all data
  /// into other buffers.
  /// The serialization always locks the part being serialized, so logging
  /// should not be paused because of a timely serialization.
  /// This function leaves out empty aggregates from the serialization to
  /// save bandwidth.
  /// \param buffer Beginning of the buffer to be fille with serialized data.
  /// \param buffer_end As _input_, this is the end of the buffer to be written.
  ///                   The buffer will not be filled past this point.
  ///                   As _output_, this is set to the actual end of the data
  ///                   which was written into the buffer which can always be
  ///                   shorter than the full buffer was.
  /// \return True if there is more data which can be written out into buffers.
  ///         False if this buffer was the last to write.
  /// \note The whole serialization is done in a thread-safe way, so this can
  ///       be incrementally done besides normal logging activities.
  bool GatherNSerialize(std::uint8_t* buffer, std::uint8_t*& buffer_end);

 private:
  using LogAggregateKey = const char*;

  static std::uint8_t* SerializeString(std::uint8_t* buf, const char* str);

  const char* data_group_name_;
  std::unordered_map<LogAggregateKey, LogAggregate> aggregates_;
  SenderLogger sender_logger_;

  std::unordered_map<LogAggregateKey, LogAggregate>::iterator serialize_next_;
};

}  // namespace pipert

#endif  // _PROFILE_DATA_H_
