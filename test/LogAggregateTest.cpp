#include "gtest/gtest.h"

#include "LogAggregate.h"

#include <cstdint>

#include "Endianness.h"

TEST(LogAggregateTest, CreatedClean) {
  pipert::LogAggregate la;
  EXPECT_EQ(la.GetLogCount(), 0);
  EXPECT_GE(la.GetTimePassedInMicroSecs(), 0);
  EXPECT_EQ(la.GetAvg(), 0.0);
}

TEST(LogAggregateTest, OneValueAggregated) {
  pipert::LogAggregate la;
  la.Log(1.0);
  EXPECT_EQ(la.GetLogCount(), 1);
  EXPECT_EQ(la.GetAvg(), 1.0);
  EXPECT_EQ(la.GetMin(), 1.0);
  EXPECT_EQ(la.GetMax(), 1.0);
  EXPECT_GE(la.GetTimePassedInMicroSecs(), 0);
}

TEST(LogAggregateTest, MoreValuesAggregated) {
  pipert::LogAggregate la;
  la.Log(1.0);
  la.Log(2.0);
  la.Log(3.0);
  EXPECT_EQ(la.GetLogCount(), 3);
  EXPECT_EQ(la.GetAvg(), 2.0);
  EXPECT_EQ(la.GetMin(), 1.0);
  EXPECT_EQ(la.GetMax(), 3.0);
  EXPECT_GE(la.GetTimePassedInMicroSecs(), 0);
}

TEST(LogAggregateTest, DataCleared) {
  pipert::LogAggregate la;
  la.Log(1.0);
  la.Log(2.0);
  EXPECT_EQ(la.GetLogCount(), 2);
  la.Clear();
  EXPECT_EQ(la.GetLogCount(), 0);
  EXPECT_GE(la.GetTimePassedInMicroSecs(), 0);
  EXPECT_EQ(la.GetAvg(), 0.0);
  la.Log(3.0);
  EXPECT_EQ(la.GetLogCount(), 1);
  EXPECT_EQ(la.GetAvg(), 3.0);
  EXPECT_EQ(la.GetMin(), 3.0);
  EXPECT_EQ(la.GetMax(), 3.0);
  EXPECT_GE(la.GetTimePassedInMicroSecs(), 0);
}

TEST(LogAggregateTest, SerializedCorrectly) {
  pipert::LogAggregate la;
  la.Log(1.0);
  la.Log(2.0);
  la.Log(3.0);
  std::uint8_t buf[256];
  std::uint8_t* buf_end = la.Serialize(buf);
  ASSERT_EQ(buf_end - buf, 32);
  EXPECT_EQ(pipert::ConvertNetworkToHostByteOrder(*(std::int32_t*)buf), 3);
  EXPECT_GE(pipert::ConvertNetworkToHostByteOrder(*(std::int32_t*)(buf + 4)),
            0);
  EXPECT_EQ(pipert::ConvertNetworkToHostByteOrder(*(double*)(buf + 8)), 1.0);
  EXPECT_EQ(pipert::ConvertNetworkToHostByteOrder(*(double*)(buf + 16)), 3.0);
  EXPECT_EQ(pipert::ConvertNetworkToHostByteOrder(*(double*)(buf + 24)), 2.0);
}

TEST(LogAggregateTest, AtomicGatheringOfData) {
  pipert::LogAggregate la;
  la.Log(1.0);
  la.Log(2.0);
  la.Log(3.0);
  std::uint8_t buf[256];
  std::uint8_t* buf_end = la.SerializeNClear(buf);
  ASSERT_EQ(buf_end - buf, 32);
  EXPECT_EQ(pipert::ConvertNetworkToHostByteOrder(*(std::int32_t*)buf), 3);
  EXPECT_GE(pipert::ConvertNetworkToHostByteOrder(*(std::int32_t*)(buf + 4)),
            0);
  EXPECT_EQ(pipert::ConvertNetworkToHostByteOrder(*(double*)(buf + 8)), 1.0);
  EXPECT_EQ(pipert::ConvertNetworkToHostByteOrder(*(double*)(buf + 16)), 3.0);
  EXPECT_EQ(pipert::ConvertNetworkToHostByteOrder(*(double*)(buf + 24)), 2.0);
  EXPECT_EQ(la.GetLogCount(), 0);
  EXPECT_GE(la.GetTimePassedInMicroSecs(), 0);
  EXPECT_EQ(la.GetAvg(), 0.0);
}
