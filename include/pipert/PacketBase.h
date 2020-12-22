#ifndef _PACKETBASE_H_
#define _PACKETBASE_H_

#include "pipert/Timer.h"

namespace pipert {

class ChannelImpl;

/// Template free base of Packet to contain all metadata about the data
/// being carried.
///
/// This is basically a raw Packet without information on the contained data.
/// See Packet for more detail.
class PacketBase {
 public:
  PacketBase(const PacketBase&) = delete;
  PacketBase& operator=(const PacketBase&) = delete;

  /// The base of scheduling work and meeting deadlines in the Scheduler.
  /// Keep in mind that this has to always resemble the time of the original
  /// data even if data is split, merged or otherwise transformed in time.
  /// \returns The exact time when the phenomenon has happened which this
  ///          data or the beginning of the buffer is a result of
  ///          (e.g. recording time, timecode of buffer start).
  ///          When transforming data, this timestamp has to be kept in order
  ///          to facilitate the correct, real-time order of scheduling.
  Timer::Time timestamp() const { return timestamp_; }

 protected:
  PacketBase(Timer::Time timestamp) : timestamp_(timestamp) {};

 private:
  Timer::Time timestamp_;  ///< Time when data was recorded.
  Timer::Time op_timestamp_;  ///< Time when operation started on data.
  friend ChannelImpl;
};

}  // namespace pipert

#endif  //_PACKETBASE_H_