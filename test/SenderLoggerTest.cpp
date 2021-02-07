#include "gtest/gtest.h"

#include "SenderLogger.h"

#include <cstring>
#include <thread>

#include "ThreadId.h"

namespace {

static const char* const kEventPushed = "Packet Pushed";
static const char* const kOtherEvent = "Mass Turbulence";

static const char* const kChannel1 = "Channel1";
static const char* const kChannel2 = "Channel2";
static const char* const kChannel3 = "Channel3";
static const char* const kChannel4 = "Channel4";
static const char* const kChannel5 =
    "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod "
    "tempor incididunt ut labore et dolore magna aliqua.";

void Simulator(const char* event_name, const char* sender_name,
               pipert::SenderLogger* sl) {
  pipert::ThreadId::TagCurrentThread();
  pipert::ThreadId::SetNameOfCurrentThread(sender_name);
  sl->TrackSender(event_name);
}

void SimulateSender(const char* event_name, const char* sender_name,
                    pipert::SenderLogger& sl) {
  std::thread t(&Simulator, event_name, sender_name, &sl);
  t.join();
}

}  // namespace

TEST(SenderLoggerTest, StartsEmpty) {
  pipert::SenderLogger sl(kEventPushed);
  EXPECT_EQ(sl.GetTopSender(), nullptr);
}

TEST(SenderLoggerTest, CorrectEventRegistered) {
  pipert::SenderLogger sl(kEventPushed);
  SimulateSender(kEventPushed, kChannel1, sl);
  EXPECT_EQ(sl.GetTopSender(), kChannel1);
}

TEST(SenderLoggerTest, OtherEventNotRegistered) {
  pipert::SenderLogger sl(kEventPushed);
  SimulateSender(kOtherEvent, kChannel1, sl);
  EXPECT_EQ(sl.GetTopSender(), nullptr);
}

TEST(SenderLoggerTest, ClearWorks) {
  pipert::SenderLogger sl(kEventPushed);
  SimulateSender(kEventPushed, kChannel1, sl);
  sl.ClearTopSenders();
  EXPECT_EQ(sl.GetTopSender(), nullptr);
}

TEST(SenderLoggerTest, TwoSendersOrderedCorrectly) {
  pipert::SenderLogger sl(kEventPushed);
  SimulateSender(kEventPushed, kChannel1, sl);
  SimulateSender(kEventPushed, kChannel2, sl);
  SimulateSender(kEventPushed, kChannel1, sl);
  EXPECT_EQ(sl.GetTopSender(), kChannel1);
}

TEST(SenderLoggerTest, ThreeSendersOrderedCorrectly) {
  pipert::SenderLogger sl(kEventPushed);
  SimulateSender(kEventPushed, kChannel1, sl);
  SimulateSender(kEventPushed, kChannel2, sl);
  SimulateSender(kEventPushed, kChannel3, sl);
  SimulateSender(kEventPushed, kChannel3, sl);
  SimulateSender(kOtherEvent, kChannel2, sl);
  SimulateSender(kOtherEvent, kChannel2, sl);
  EXPECT_EQ(sl.GetTopSender(), kChannel3);
}

TEST(SenderLoggerTest, LotSendersOrderedCorrectly) {
  pipert::SenderLogger sl(kEventPushed);
  SimulateSender(kEventPushed, kChannel1, sl);
  SimulateSender(kEventPushed, kChannel2, sl);
  SimulateSender(kEventPushed, kChannel3, sl);
  SimulateSender(kEventPushed, kChannel4, sl);
  SimulateSender(kEventPushed, kChannel5, sl);
  SimulateSender(kEventPushed, kChannel5, sl);
  SimulateSender(kEventPushed, kChannel2, sl);
  EXPECT_EQ(sl.GetTopSender(), kChannel2);
}

TEST(SenderLoggerTest, SerializedCorrectly) {
  pipert::SenderLogger sl(kEventPushed);
  SimulateSender(kEventPushed, kChannel1, sl);
  std::uint8_t buf[256];
  std::uint8_t* buf_end = sl.Serialize(buf);
  ASSERT_EQ(buf_end - buf, 4 + strlen(kChannel1) + 1);
  EXPECT_EQ(memcmp(buf, "SEND", 4), 0);
  EXPECT_EQ(memcmp(buf + 4, kChannel1, strlen(kChannel1)), 0);
  EXPECT_EQ(buf_end[-1], 0);
}

TEST(SenderLoggerTest, EmptySerializedCorrectly) {
  pipert::SenderLogger sl(kEventPushed);
  std::uint8_t buf[256];
  std::uint8_t* buf_end = sl.Serialize(buf);
  ASSERT_EQ(buf_end - buf, 4 + strlen("N/A") + 1);
  EXPECT_EQ(memcmp(buf, "SEND", 4), 0);
  EXPECT_EQ(buf_end[-1], 0);
}

TEST(SenderLoggerTest, LongNameSerializedCorrectly) {
  pipert::SenderLogger sl(kEventPushed);
  SimulateSender(kEventPushed, kChannel5, sl);
  std::uint8_t buf[256];
  std::uint8_t* buf_end = sl.Serialize(buf);
  ASSERT_EQ(buf_end - buf, 4 + pipert::SenderLogger::kMaxSenderNameLength + 1);
  EXPECT_EQ(memcmp(buf, "SEND", 4), 0);
  EXPECT_EQ(
      memcmp(buf + 4, kChannel5, pipert::SenderLogger::kMaxSenderNameLength),
      0);
  EXPECT_EQ(buf_end[-1], 0);
}

TEST(SenderLoggerTest, AtomicSerialization) {
  pipert::SenderLogger sl(kEventPushed);
  SimulateSender(kEventPushed, kChannel1, sl);
  std::uint8_t buf[256];
  std::uint8_t* buf_end = sl.SerializeNClear(buf);
  ASSERT_EQ(buf_end - buf, 4 + strlen(kChannel1) + 1);
  EXPECT_EQ(buf_end[-1], 0);
  EXPECT_EQ(sl.GetTopSender(), nullptr);
}
