#ifndef _PACKETBASE_H_
#define _PACKETBASE_H_

#include "pipert/Timer.h"

namespace pipert {

class PacketBase {
 public:
  PacketBase(const PacketBase&) = delete;
  PacketBase& operator=(const PacketBase&) = delete;

  Timer::Time timestamp() const { return timestamp_; }

 protected:
  PacketBase(Timer::Time timestamp) : timestamp_(timestamp) {};

 private:
  Timer::Time timestamp_;  ///< time when data came into the system
};

}  // namespace pipert

#endif  //_PACKETBASE_H_