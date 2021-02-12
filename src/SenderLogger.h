#ifndef _SENDER_LOGGER_H_
#define _SENDER_LOGGER_H_

#include <array>
#include <cstdint>

#include "AdaptiveSpinLock.h"

namespace pipert {

/// (_Part of internal implementation._)
/// Tracks the sender object of the packet by finding out the name of the
/// currently executing thread.
/// A couple of names are logged and ordered to find the most frequent sender.
/// This object can serialize its data for log collection.
class SenderLogger {
 public:
  /// Maximum length of sender name that will be serialized.
  static const int kMaxSenderNameLength = 27;

  /// Constructs a SenderLogger object.
  /// \param track_event The event name in which data is sent to this object.
  ///                    This event will be used for tracking.
  ///                    Other event names are discarded in sender tracking.
  SenderLogger(const char* track_event);
  SenderLogger(const SenderLogger&) = delete;
  SenderLogger& operator=(const SenderLogger&) = delete;

  /// Finds out the name of the sender object by inspecting the currently
  /// running thread.
  /// This can be used in a multi-threaded scenario from outside.
  /// \param event_name The name of the event which is being logged.
  /// \note This is a thread-safe function having an object-local granularity.
  void TrackSender(const char* event_name);

  /// Serializes and clears data gathered so far.
  /// This can be used in a multi-threaded scenario from outside.
  /// \param buffer Pointer to serialization buffer where data should be put.
  /// \return Pointer to the place in the buffer after the serialized data.
  /// \note This is a thread-safe function having an object-local granularity.
  std::uint8_t* SerializeNClear(std::uint8_t* buffer);

  /// Serializes data into a buffer.
  ///
  /// The serialized data has a header of 4 bytes "SEND".
  /// \see SerializeNClear().
  /// \note This function is NOT thread-safe and is better to be used
  ///       internally. It serializes "N/A" as name when there was no sender.
  std::uint8_t* Serialize(std::uint8_t* buffer);

  /// Clears data gathered so far.
  /// \note This function is NOT thread-safe and is better to be used
  ///       internally.
  void ClearTopSenders();

  /// Gets the sender which sent the most packets.
  /// Only `kMaxSenders` amount of senders can be kept at once, but a
  /// low number should be practically enough to track most scenarios.
  /// \return The name of the most frequent sender.
  /// \note This function is NOT thread-safe and is better to be used
  ///       internally.
  const char* GetTopSender();

 private:
  /// Number of senders to be tracked bz this object.
  static const int kMaxSenders = 4;

  struct Sender {
    bool operator<(const Sender& o) const {
      return sent_packets_ > o.sent_packets_;
    }

    const char* sender_name_;
    int sent_packets_;
  };

  const char* track_event_;
  AdaptiveSpinLock mutex_;
  std::array<Sender, kMaxSenders> senders_;
};

}  // namespace pipert

#endif  // _SENDER_LOGGER_H_
