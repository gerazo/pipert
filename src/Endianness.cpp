#include "Endianness.h"

namespace pipert {

template <>
float ConvertHostToNetworkByteOrder<float>(float val) {
  static_assert(sizeof(float) == sizeof(std::uint32_t), "Weird float size");
  union {
    float f;
    std::uint32_t ui32;
  } ret;
  ret.f = val;
  ret.ui32 = ConvertHostToNetworkByteOrder(ret.ui32);
  return ret.f;
}

template <>
double ConvertHostToNetworkByteOrder<double>(double val) {
  static_assert(sizeof(double) == sizeof(std::uint64_t), "Weird double size");
  union {
    double d;
    std::uint64_t ui64;
  } ret;
  ret.d = val;
  ret.ui64 = ConvertHostToNetworkByteOrder(ret.ui64);
  return ret.d;
}

template <>
float ConvertNetworkToHostByteOrder<float>(float val) {
  static_assert(sizeof(float) == sizeof(std::uint32_t), "Weird float size");
  union {
    float f;
    std::uint32_t ui32;
  } ret;
  ret.f = val;
  ret.ui32 = ConvertNetworkToHostByteOrder(ret.ui32);
  return ret.f;
}

template <>
double ConvertNetworkToHostByteOrder<double>(double val) {
  static_assert(sizeof(double) == sizeof(std::uint64_t), "Weird double size");
  union {
    double d;
    std::uint64_t ui64;
  } ret;
  ret.d = val;
  ret.ui64 = ConvertNetworkToHostByteOrder(ret.ui64);
  return ret.d;
}

}  // namespace pipert
