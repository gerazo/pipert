#include "gtest/gtest.h"

#include "Endianness.h"

namespace {

template <typename T>
class EndiannessTest : public testing::Test {};

using TestTypes =
    ::testing::Types<char, signed char, unsigned char, short, unsigned short,
                     int, unsigned int, long, unsigned long, long long,
                     unsigned long long, float, double, std::uint8_t,
                     std::uint16_t, std::uint32_t, std::uint64_t, std::int8_t,
                     std::int16_t, std::int32_t, std::int64_t>;
TYPED_TEST_SUITE(EndiannessTest, TestTypes, );

}  // namespace

template <typename T>
void TestFullConversion(T val) {
  EXPECT_EQ(pipert::ConvertNetworkToHostByteOrder(
                pipert::ConvertHostToNetworkByteOrder(val)),
            val);
  EXPECT_EQ(pipert::ConvertHostToNetworkByteOrder(
                pipert::ConvertNetworkToHostByteOrder(val)),
            val);
}

TYPED_TEST(EndiannessTest, FullConversionTest) {
  TestFullConversion<TypeParam>(0);
  TestFullConversion<TypeParam>(-1);
  TestFullConversion<TypeParam>(1);
  TestFullConversion<TypeParam>(-77);
  TestFullConversion<TypeParam>(77);
}
