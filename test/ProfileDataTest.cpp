#include "ProfileData.h"
#include "gtest/gtest.h"

#include <cstring>

#include "pipert/LogEvent.h"

namespace {

const int kUDPMaxPacketSize = 508;
const char channel_name[] = "my-channnel";

}  // namespace

TEST(ProfileDataTest, Naming) {
  pipert::ProfileData pd(channel_name);
  EXPECT_STREQ(pd.GetName(), channel_name);
}

TEST(ProfileDataTest, SerializeEmpty) {
  pipert::ProfileData pd(channel_name);
  std::uint8_t buf[kUDPMaxPacketSize];
  std::uint8_t* buf_end = buf + kUDPMaxPacketSize;
  bool more = pd.GatherNSerialize(buf, buf_end);
  EXPECT_FALSE(more);
  EXPECT_EQ(memcmp(buf, "DGRP", 4), 0);
  EXPECT_STREQ((const char*)buf + 4, channel_name);
  EXPECT_EQ(memcmp(buf + 4 + sizeof(channel_name), "SEND", 4), 0);
  EXPECT_STREQ((const char*)buf + 4 + sizeof(channel_name) + 4, "N/A");
  EXPECT_EQ(buf_end, buf + 4 + sizeof(channel_name) + 4 + sizeof("N/A"));
}

TEST(ProfileDataTest, OneTypeLogged) {
  pipert::ProfileData pd(channel_name);
  pd.Log(pipert::LogEvent<pipert::ProfileData::kEventPushed>(100.0));
  std::uint8_t buf[kUDPMaxPacketSize];
  std::uint8_t* buf_end = buf + kUDPMaxPacketSize;
  bool more = pd.GatherNSerialize(buf, buf_end);
  EXPECT_FALSE(more);
  std::uint8_t* p = buf + 4 + sizeof(channel_name) + 4 + sizeof("N/A");
  EXPECT_EQ(memcmp(p, "LOGA", 4), 0);
  EXPECT_STREQ((const char*)p + 4, pipert::ProfileData::kEventPushed);
  EXPECT_EQ(buf_end,
            p + 4 + strlen(pipert::ProfileData::kEventPushed) + 1 + 32);
}

TEST(ProfileDataTest, MoreTypesLoggedIntoMorePackets) {
  pipert::ProfileData pd(channel_name);
  pd.Log(pipert::LogEvent<pipert::ProfileData::kEventPushed>(100.0));
  pd.Log(pipert::LogEvent<pipert::ProfileData::kEventRetrieved>(110.0));
  pd.Log(pipert::LogEvent<pipert::ProfileData::kEventExecuteTime>(5.0));
  pd.Log(pipert::LogEvent<pipert::ProfileData::kEventFillTime>(6.0));
  pd.Log(pipert::LogEvent<pipert::ProfileData::kEventReadTime>(7.0));
  const int kSmallBufferSize = 128;
  std::uint8_t buf[kSmallBufferSize];
  std::uint8_t* buf_end;
  bool more = true;
  int packets = 0;
  int events = 0;
  while (more && packets < 9) {
    buf_end = buf + kSmallBufferSize;
    more = pd.GatherNSerialize(buf, buf_end);
    EXPECT_EQ(memcmp(buf, "DGRP", 4), 0);
    EXPECT_STREQ((const char*)buf + 4, channel_name);
    std::uint8_t* p = buf + 4 + sizeof(channel_name);
    if (events == 0) {
      EXPECT_EQ(memcmp(p, "SEND", 4), 0);
      EXPECT_STREQ((const char*)p + 4, "N/A");
      p += 4 + sizeof("N/A");
    }
    while (p < buf_end) {
      EXPECT_EQ(memcmp(p, "LOGA", 4), 0);
      int len = strlen((const char*)p + 4);
      p += 4 + len + 1 + 32;
      events++;
    }
    EXPECT_EQ(p, buf_end);
    packets++;
  }
  EXPECT_FALSE(more);
  EXPECT_LE(packets, 9);
  EXPECT_EQ(events, 5);
}
