#include "gtest/gtest.h"

#include "Endianness.h"

namespace {

template <typename T>
class EndiannessTest : public testing::Test {};

using TestTypes = ::testing::Types<char, short, unsigned short, int,
                                   unsigned int, long, unsigned long, long long,
                                   unsigned long long, float, double>;
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
