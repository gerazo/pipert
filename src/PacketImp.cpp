#include "pipert/Timer.h"
#include "pipert/src/PacketImp.h"

namespace pipert {

PacketImp::PacketImp(const Byte* data, size_t packet_size) 
      : created_at_(Timer::time()) {
  raw_data_.resize(packet_size, 0);
  const Byte* end = data + packet_size;
  std::copy(data, end, std::begin(raw_data_));
}

const Byte* PacketImp::GetData() const {
  return &raw_data_[0];
}

Byte* PacketImp::GetData() {
  return &raw_data_[0];
}

Time PacketImp::GetCreatedTime() const {
  return created_at_;
}
 
}  // namespace pipert