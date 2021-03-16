#include "gtest/gtest.h"
#include "pipert/PolledChannel.h"
#include "pipert/Scheduler.h"

class PolledChannelTest : public ::testing::Test {
 protected:
  ::pipert::Scheduler scheduler_;
  ::pipert::Timer::Time default_time_;
  ::pipert::Timer::Time old_time_;
  ::pipert::Timer::Time new_time_;

  PolledChannelTest()
      : scheduler_(1),
        default_time_(1603716824924000),
        old_time_(1603716857718209),
        new_time_(1603716871368358) {}

  virtual ~PolledChannelTest() {}

  virtual void SetUp() {}

  virtual void TearDown() {}
};

TEST_F(PolledChannelTest, PolledChannelCreationTestWithScheduler) {
  ::pipert::PolledChannel<int> polled_channel =
      scheduler_.CreatePolledChannel<int>("TestChannel", 2);
  EXPECT_EQ(polled_channel.GetName(), "TestChannel");
  EXPECT_EQ(polled_channel.GetCapacity(), 2);
  EXPECT_EQ(polled_channel.GetPacketSize(), sizeof(::pipert::Packet<int>));
}

TEST_F(PolledChannelTest, PolledChannelAcquireTest) {
  int channel_capacity = 2;
  ::pipert::PolledChannel<int> polled_channel =
      scheduler_.CreatePolledChannel<int>("TestChannel", channel_capacity);
  EXPECT_EQ(polled_channel.GetFreeBufferLength(), channel_capacity);
  EXPECT_EQ(polled_channel.GetQueuedBufferLength(), 0);
  int value = 42;
  ::pipert::PacketToFill<int> packet =
      polled_channel.Acquire(default_time_, value);
  EXPECT_EQ(polled_channel.GetFreeBufferLength(), channel_capacity - 1);
  EXPECT_EQ(polled_channel.GetQueuedBufferLength(), 0);
  EXPECT_EQ(packet.data(), value);
  EXPECT_EQ(packet.timestamp(), default_time_);
}

TEST_F(PolledChannelTest, PolledChannelPushPacketToFillTest) {
  int channel_capacity = 2;
  ::pipert::PolledChannel<int> polled_channel =
      scheduler_.CreatePolledChannel<int>("TestChannel", channel_capacity);
  ::pipert::PacketToFill<int> packet_to_fill =
      polled_channel.Acquire(default_time_, 42);
  EXPECT_EQ(polled_channel.GetFreeBufferLength(), channel_capacity - 1);
  EXPECT_EQ(polled_channel.GetQueuedBufferLength(), 0);
  packet_to_fill.Push();
  EXPECT_EQ(polled_channel.GetFreeBufferLength(), channel_capacity - 1);
  EXPECT_EQ(polled_channel.GetQueuedBufferLength(), 1);
  EXPECT_TRUE(packet_to_fill.IsEmpty());
}

TEST_F(PolledChannelTest, PolledChannelAutoPushPacketToFillTest) {
  int channel_capacity = 2;
  ::pipert::PolledChannel<int> polled_channel =
      scheduler_.CreatePolledChannel<int>("TestChannel", channel_capacity);
  {
    ::pipert::PacketToFill<int> packet_to_fill =
        polled_channel.Acquire(default_time_, 42);
    EXPECT_EQ(polled_channel.GetFreeBufferLength(), channel_capacity - 1);
    EXPECT_EQ(polled_channel.GetQueuedBufferLength(), 0);
  }
  EXPECT_EQ(polled_channel.GetFreeBufferLength(), channel_capacity - 1);
  EXPECT_EQ(polled_channel.GetQueuedBufferLength(), 1);
}

TEST_F(PolledChannelTest,
       PolledChannelPollPacketToProcessWithEmptyChannelTest) {
  int channel_capacity = 2;
  ::pipert::PolledChannel<int> polled_channel =
      scheduler_.CreatePolledChannel<int>("TestChannel", channel_capacity);
  ::pipert::PacketToProcess<int> packet_to_process = polled_channel.Poll();
  EXPECT_TRUE(packet_to_process.GetPacket() == nullptr);
  EXPECT_TRUE(packet_to_process.IsEmpty());
}

TEST_F(PolledChannelTest, PolledChannelPollPacketToProcessTest) {
  int channel_capacity = 2;
  ::pipert::PolledChannel<int> polled_channel =
      scheduler_.CreatePolledChannel<int>("TestChannel", channel_capacity);
  ::pipert::PacketToFill<int> packet_to_fill =
      polled_channel.Acquire(default_time_, 42);
  packet_to_fill.Push();
  EXPECT_EQ(polled_channel.GetFreeBufferLength(), channel_capacity - 1);
  EXPECT_EQ(polled_channel.GetQueuedBufferLength(), 1);
  ::pipert::PacketToProcess<int> packet_to_process = polled_channel.Poll();
  EXPECT_EQ(polled_channel.GetFreeBufferLength(), channel_capacity - 1);
  EXPECT_EQ(polled_channel.GetQueuedBufferLength(), 0);
  EXPECT_FALSE(packet_to_process.IsEmpty());
  EXPECT_EQ(packet_to_process.data(), 42);
}

TEST_F(PolledChannelTest, PolledChannelPollPacketToProcessWithReleaseTest) {
  int channel_capacity = 2;
  ::pipert::PolledChannel<int> polled_channel =
      scheduler_.CreatePolledChannel<int>("TestChannel", channel_capacity);
  ::pipert::PacketToFill<int> packet_to_fill =
      polled_channel.Acquire(default_time_, 42);
  packet_to_fill.Push();
  ::pipert::PacketToProcess<int> packet_to_process = polled_channel.Poll();
  EXPECT_EQ(polled_channel.GetFreeBufferLength(), channel_capacity - 1);
  EXPECT_EQ(polled_channel.GetQueuedBufferLength(), 0);
  packet_to_process.Release();
  EXPECT_TRUE(packet_to_process.IsEmpty());
  EXPECT_EQ(polled_channel.GetFreeBufferLength(), channel_capacity);
  EXPECT_EQ(polled_channel.GetQueuedBufferLength(), 0);
}

TEST_F(PolledChannelTest, PolledChannelPollPacketToProcessWithAutoReleaseTest) {
  int channel_capacity = 2;
  ::pipert::PolledChannel<int> polled_channel =
      scheduler_.CreatePolledChannel<int>("TestChannel", channel_capacity);
  ::pipert::PacketToFill<int> packet_to_fill =
      polled_channel.Acquire(default_time_, 42);
  packet_to_fill.Push();
  {
    ::pipert::PacketToProcess<int> packet_to_process = polled_channel.Poll();
    EXPECT_EQ(polled_channel.GetFreeBufferLength(), channel_capacity - 1);
    EXPECT_EQ(polled_channel.GetQueuedBufferLength(), 0);
  }
  EXPECT_EQ(polled_channel.GetFreeBufferLength(), channel_capacity);
  EXPECT_EQ(polled_channel.GetQueuedBufferLength(), 0);
}

TEST_F(PolledChannelTest, PolledChannelDropPacketAfterPushTest) {
  int channel_capacity = 1;
  ::pipert::PolledChannel<int> polled_channel =
      scheduler_.CreatePolledChannel<int>("TestChannel", channel_capacity);
  ::pipert::PacketToFill<int> packet_to_fill =
      polled_channel.Acquire(old_time_, 32);
  packet_to_fill.Push();
  packet_to_fill = polled_channel.Acquire(new_time_, 42);
  packet_to_fill.Push();
  ::pipert::PacketToProcess<int> packet_to_process = polled_channel.Poll();
  EXPECT_FALSE(packet_to_process.IsEmpty());
  EXPECT_EQ(packet_to_process.data(), 42);
  EXPECT_EQ(packet_to_process.timestamp(), new_time_);
  packet_to_process.Release();
  EXPECT_EQ(polled_channel.GetFreeBufferLength(), channel_capacity);
  EXPECT_EQ(polled_channel.GetQueuedBufferLength(), 0);
  EXPECT_TRUE(packet_to_process.IsEmpty());
}

TEST_F(PolledChannelTest, PolledChannelDropPacketBeforePushTest) {
  int channel_capacity = 1;
  ::pipert::PolledChannel<int> polled_channel =
      scheduler_.CreatePolledChannel<int>("TestChannel", channel_capacity);
  ::pipert::PacketToFill<int> packet_to_fill_old =
      polled_channel.Acquire(old_time_, 32);
  ::pipert::PacketToFill<int> packet_to_fill_new =
      polled_channel.Acquire(new_time_, 42);
  EXPECT_TRUE(packet_to_fill_new.IsEmpty());
  packet_to_fill_old.Push();
  ::pipert::PacketToProcess<int> packet_to_process = polled_channel.Poll();
  EXPECT_FALSE(packet_to_process.IsEmpty());
  EXPECT_EQ(packet_to_process.data(), 32);
  EXPECT_EQ(packet_to_process.timestamp(), old_time_);
  packet_to_process.Release();
  EXPECT_EQ(polled_channel.GetFreeBufferLength(), channel_capacity);
  EXPECT_EQ(polled_channel.GetQueuedBufferLength(), 0);
  EXPECT_TRUE(packet_to_process.IsEmpty());
}
