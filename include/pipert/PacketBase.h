#ifndef _PACKETBASE_H_
#define _PACKETBASE_H_

#include "pipert/Timer.h"

namespace pipert {

/// Template free base of Packet to contain all metadata about the data
/// being carried.
///
/// See Packet for more detail.
class PacketBase {
 public:
  PacketBase(const PacketBase&) = delete;
  PacketBase& operator=(const PacketBase&) = delete;

  /// The base of scheduling work and meeting deadlines in the Scheduler.
  /// \returns The exact time when the phenomenon has happened which this
  ///          data is a result of (e.g. recording time).
  ///          When transforming data, this timestamp has to be kept in order
  ///          to facilitate the correct, real-time order of scheduling.
  Timer::Time timestamp() const { return timestamp_; }

 protected:
  PacketBase(Timer::Time timestamp) : timestamp_(timestamp) {};

 private:
  Timer::Time timestamp_;  ///< Time when data was recorded.
};

}  // namespace pipert

#endif  //_PACKETBASE_H_