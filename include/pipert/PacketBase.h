#ifndef _PACKETBASE_H_
#define _PACKETBASE_H_

#include "pipert/Timer.h"

namespace pipert {

class PacketBase {
 public:
  PacketBase(const PacketBase&) = delete;
  PacketBase& operator=(const PacketBase&) = delete;

  Timer::Time GetCreatedTime() const { return created_at_; }

 protected:
  PacketBase(Timer::Time created_at) : created_at_(created_at) {};

 private:
  Timer::Time created_at_;  // timestamp when package was first created
};

}  // namespace pipert

#endif  //_PACKETBASE_H_