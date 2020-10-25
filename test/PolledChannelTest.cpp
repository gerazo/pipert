#include "pipert/PolledChannel.h"
#include "gtest/gtest.h"

#include "pipert/Scheduler.h"

 class PolledChannelTest : public ::testing::Test {
  protected:
    pipert::Scheduler scheduler_;

    PolledChannelTest() : scheduler_(1) {
    }

    virtual ~PolledChannelTest() {
    }

    virtual void SetUp() {
    }

    virtual void TearDown() {
    }

  };

TEST_F(PolledChannelTest, PolledChannelCreationTestWithScheduler) {
  pipert::Scheduler scheduler(1);
  pipert::PolledChannel<int> polledChannel = scheduler.CreatePolledChannel<int>("TestChannel", 2);
  EXPECT_EQ(polledChannel.GetName(), "TestChannel");
  EXPECT_EQ(polledChannel.GetCapacity(), 2);
  EXPECT_EQ(polledChannel.GetPacketSize(), sizeof(pipert::Packet<int>));
}

TEST_F(PolledChannelTest, PolledChannelAcquireTest) {
  int channel_capacity = 2;
  pipert::PolledChannel<int> polledChannel = scheduler_.CreatePolledChannel<int>("TestChannel", channel_capacity);
  pipert::Timer::Time testTime(pipert::Timer::time());
  EXPECT_EQ(polledChannel.GetFreePacketSize(), channel_capacity);
  EXPECT_EQ(polledChannel.GetQueuedPacketSize(), 0);
  int value = 42;
  pipert::PacketToFill<int> packet = polledChannel.Acquire("TestChannel", testTime, value);
  EXPECT_EQ(polledChannel.GetFreePacketSize(), channel_capacity - 1);
  EXPECT_EQ(polledChannel.GetQueuedPacketSize(), 0);
  EXPECT_EQ(packet.data(), value);
  EXPECT_EQ(packet.timestamp(), testTime);
}

TEST_F(PolledChannelTest, PolledChannelPushPacketToFillTest) {
  int channel_capacity = 2;
  pipert::PolledChannel<int> polled_channel = scheduler_.CreatePolledChannel<int>("TestChannel", channel_capacity);
  pipert::PacketToFill<int> packet_to_fill = polled_channel.Acquire("TestChannel", pipert::Timer::time(), 42);
  EXPECT_EQ(polled_channel.GetFreePacketSize(), channel_capacity - 1);
  EXPECT_EQ(polled_channel.GetQueuedPacketSize(), 0);
  packet_to_fill.Push();
  EXPECT_EQ(polled_channel.GetFreePacketSize(), channel_capacity - 1);
  EXPECT_EQ(polled_channel.GetQueuedPacketSize(), 1);
  EXPECT_TRUE(packet_to_fill.IsEmpty());
}

TEST_F(PolledChannelTest, PolledChannelAutoPushPacketToFillTest) {
  int channel_capacity = 2;
  pipert::PolledChannel<int> polled_channel = scheduler_.CreatePolledChannel<int>("TestChannel", channel_capacity);
  {
    pipert::PacketToFill<int> packet_to_fill = polled_channel.Acquire("TestChannel", pipert::Timer::time(), 42);
    EXPECT_EQ(polled_channel.GetFreePacketSize(), channel_capacity - 1);
    EXPECT_EQ(polled_channel.GetQueuedPacketSize(), 0);
  }
  EXPECT_EQ(polled_channel.GetFreePacketSize(), channel_capacity - 1);
  EXPECT_EQ(polled_channel.GetQueuedPacketSize(), 1);
}

TEST_F(PolledChannelTest, PolledChannelPollPacketToProcessWithEmptyChannelTest) {
  int channel_capacity = 2;
  pipert::PolledChannel<int> polled_channel = scheduler_.CreatePolledChannel<int>("TestChannel", channel_capacity);
  pipert::PacketToProcess<int> packet_to_process = polled_channel.Poll();
  #ifdef DEBUG
  EXPECT_DEATH(packet_to_process.data(), ".*");
  #endif
  EXPECT_TRUE(packet_to_process.GetPacket() == nullptr);
  EXPECT_TRUE(packet_to_process.IsEmpty());
}

TEST_F(PolledChannelTest, PolledChannelPollPacketToProcessTest) {
  int channel_capacity = 2;
  pipert::PolledChannel<int> polled_channel = scheduler_.CreatePolledChannel<int>("TestChannel", channel_capacity);
  pipert::PacketToFill<int> packet_to_fill = polled_channel.Acquire("TestChannel", pipert::Timer::time(), 42);
  packet_to_fill.Push();
  EXPECT_EQ(polled_channel.GetFreePacketSize(), channel_capacity - 1);
  EXPECT_EQ(polled_channel.GetQueuedPacketSize(), 1);
  pipert::PacketToProcess<int> packet_to_process = polled_channel.Poll();
  EXPECT_EQ(polled_channel.GetFreePacketSize(), channel_capacity - 1);
  EXPECT_EQ(polled_channel.GetQueuedPacketSize(), 0);
  EXPECT_FALSE(packet_to_process.IsEmpty());
  EXPECT_EQ(packet_to_process.data(), 42);
}

TEST_F(PolledChannelTest, PolledChannelPollPacketToProcessWithReleaseTest) {
  int channel_capacity = 2;
  pipert::PolledChannel<int> polled_channel = scheduler_.CreatePolledChannel<int>("TestChannel", channel_capacity);
  pipert::PacketToFill<int> packet_to_fill = polled_channel.Acquire("TestChannel", pipert::Timer::time(), 42);
  packet_to_fill.Push();
  pipert::PacketToProcess<int> packet_to_process = polled_channel.Poll();
  EXPECT_EQ(polled_channel.GetFreePacketSize(), channel_capacity - 1);
  EXPECT_EQ(polled_channel.GetQueuedPacketSize(), 0);
  packet_to_process.Release();
  EXPECT_TRUE(packet_to_process.IsEmpty());
  EXPECT_EQ(polled_channel.GetFreePacketSize(), channel_capacity);
  EXPECT_EQ(polled_channel.GetQueuedPacketSize(), 0);
}

TEST_F(PolledChannelTest, PolledChannelPollPacketToProcessWithAutoReleaseTest) {
  int channel_capacity = 2;
  pipert::PolledChannel<int> polled_channel = scheduler_.CreatePolledChannel<int>("TestChannel", channel_capacity);
  pipert::PacketToFill<int> packet_to_fill = polled_channel.Acquire("TestChannel", pipert::Timer::time(), 42);
  packet_to_fill.Push();
  {
    pipert::PacketToProcess<int> packet_to_process = polled_channel.Poll();
    EXPECT_EQ(polled_channel.GetFreePacketSize(), channel_capacity - 1);
    EXPECT_EQ(polled_channel.GetQueuedPacketSize(), 0);
  }
  EXPECT_EQ(polled_channel.GetFreePacketSize(), channel_capacity);
  EXPECT_EQ(polled_channel.GetQueuedPacketSize(), 0);
}