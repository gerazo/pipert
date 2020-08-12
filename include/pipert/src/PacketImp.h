#ifndef _PACKET_IMP_H_
#define _PACKET_IMP_H_

#include <vector>

namespace pipert {

using Byte = unsigned char;

class PacketImp {
 public:
  PacketImp(const Byte* data, size_t packet_size);

  ~PacketImp() {}

  const Byte* GetData() const;
  Byte* GetData();

  Timer::Time GetCreatedTime() const;

 private:
  Timer::Time created_at_;  // timestamp when package was first created
  std::vector<Byte> raw_data_;       // the data stored in the package
};

}  // namespace pipert

#endif  //_PACKET_IMP_H_