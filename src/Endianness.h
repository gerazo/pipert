#ifndef _ENDIANNESS_H_
#define _ENDIANNESS_H_

#include <climits>
#include <cstdint>

namespace pipert {

/// (_Part of internal implementation._)
/// Converts primitive, built-in types from native host byte ordering to
/// network (MSB) byte ordering in a platform independent way.
/// \param val Value to be converted.
/// \return Value having its internal byte order changed if it was necessary.
template <typename T>
T ConvertHostToNetworkByteOrder(T val) {
  static_assert(CHAR_BIT == 8,
                "8-bit types should be available on current platform");
  T ret = 0;
  std::uint8_t* bytes = (std::uint8_t*)&ret;
  for (int i = 0; i < (int)sizeof(T); i++) {
    bytes[i] = (std::uint8_t)(val >> (sizeof(T) * 8 - 8));
    val <<= 8;
  }
  return ret;
}

/// (_Part of internal implementation._)
/// Specialized version of ConvertHostToNetworkByteOrder() for floats.
template <>
float ConvertHostToNetworkByteOrder<float>(float val);

/// (_Part of internal implementation._)
/// Specialized version of ConvertHostToNetworkByteOrder() for doubles.
template <>
double ConvertHostToNetworkByteOrder<double>(double val);

/// (_Part of internal implementation._)
/// Converts primitive, built-in types from network (MSB) byte ordering to
/// native host byte ordering in a platform independent way.
/// \param val Value to be converted.
/// \return Value having its internal byte order changed if it was necessary.
template <typename T>
T ConvertNetworkToHostByteOrder(T val) {
  static_assert(CHAR_BIT == 8,
                "8-bit types should be available on current platform");
  T ret = 0;
  std::uint8_t* bytes = (std::uint8_t*)&val;
  for (int i = 0; i < (int)sizeof(T); i++) {
    ret = ((ret << 8) | bytes[i]);
  }
  return ret;
}

/// (_Part of internal implementation._)
/// Specialized version of ConvertNetworkToHostByteOrder() for floats.
template <>
float ConvertNetworkToHostByteOrder<float>(float val);

/// (_Part of internal implementation._)
/// Specialized version of ConvertNetworkToHostByteOrder() for doubles.
template <>
double ConvertNetworkToHostByteOrder<double>(double val);

}  // namespace pipert

#endif  // _ENDIANNESS_H_
