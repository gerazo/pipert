#include "pipert/ScheduledChannel.h"
#include "pipert/Scheduler.h"
#include "gtest/gtest.h"

namespace{
    class OtherPrinter {
        public:
        void Print(pipert::PacketToProcess<int> packet) {
            std::cout << "Other: " << packet.data() << std::endl;
        }
        void Add(pipert::PacketToProcess<int> packet) {
            std::cout << packet.data() << std::endl;
            packet.data() = 11;
        }
    };

} //namespace

class ScheduledChannelTest : public ::testing::Test {
    protected:
    ::pipert::Scheduler scheduler_;
    ::pipert::Timer::Time default_time_;
    ::pipert::Timer::Time old_time_;
    ::pipert::Timer::Time new_time_;
    OtherPrinter op_;

    ScheduledChannelTest() 
        : scheduler_(1),
        default_time_(1603716824924000),
        old_time_(1603716857718209),
        new_time_(1603716871368358) {
    }

    virtual ~ScheduledChannelTest() {
    }

    virtual void SetUp() {
    }

    virtual void TearDown() {
    }

};


TEST_F(ScheduledChannelTest, ScheduledChannelCreationWithSchedulerBasicChannelInfoTest) {
    // When
    ::pipert::ScheduledChannel<int> channel = scheduler_.CreateScheduledChannel<int>("TestChannel", 2, nullptr, std::bind(&OtherPrinter::Print, &op_, std::placeholders::_1));
    // Then
    EXPECT_EQ(channel.GetName(), "TestChannel");
    EXPECT_EQ(channel.GetCapacity(), 2);
    EXPECT_EQ(channel.GetPacketSize(), sizeof(::pipert::Packet<int>));
}

TEST_F(ScheduledChannelTest, ScheduledChannelCreationWithSchedulerBufferInfoTest) {
    // When
    ::pipert::ScheduledChannel<int> channel = scheduler_.CreateScheduledChannel<int>("TestChannel", 2, nullptr, std::bind(&OtherPrinter::Print, &op_, std::placeholders::_1));
    // Then
    EXPECT_EQ(channel.GetFreeBufferLength(), 2);
    EXPECT_EQ(channel.GetQueuedBufferLength(), 0);
}

TEST_F(ScheduledChannelTest, ScheduledChannelAcquireTest) {
    // Given
    const int value = 42;
    ::pipert::ScheduledChannel<int> channel = scheduler_.CreateScheduledChannel<int>("TestChannel", 2, nullptr, std::bind(&OtherPrinter::Print, &op_, std::placeholders::_1));
    // When
    ::pipert::PacketToFill<int> packet = channel.Acquire("TestChannel", default_time_, value);
    // Then
    EXPECT_EQ(channel.GetFreeBufferLength(), 1);
    EXPECT_EQ(channel.GetQueuedBufferLength(), 0);
    EXPECT_EQ(packet.data(), value);
    EXPECT_EQ(packet.timestamp(), default_time_);    
}

TEST_F(ScheduledChannelTest, ScheduledChannelPushPacketToFillTest) {
    // Given
    const int value = 42;
    ::pipert::ScheduledChannel<int> channel = scheduler_.CreateScheduledChannel<int>("TestChannel", 2, nullptr, std::bind(&OtherPrinter::Print, &op_, std::placeholders::_1));
    ::pipert::PacketToFill<int> packet_to_fill = channel.Acquire("TestChannel", default_time_, value);
    // When
    packet_to_fill.Push(); 
    // Then
    EXPECT_EQ(channel.GetFreeBufferLength(), 1);
    EXPECT_EQ(channel.GetQueuedBufferLength(), 1);
    EXPECT_TRUE(packet_to_fill.IsEmpty());    
}

TEST_F(ScheduledChannelTest, ScheduledChannelAutoPushPacketToFillTest) {
    // Given
    int channel_capacity = 2;
    ::pipert::ScheduledChannel<int> channel = scheduler_.CreateScheduledChannel<int>("TestChannel", 2, nullptr, std::bind(&OtherPrinter::Print, &op_, std::placeholders::_1));
    // When
    {
        ::pipert::PacketToFill<int> packet_to_fill = channel.Acquire("TestChannel", default_time_, 42);
        EXPECT_EQ(channel.GetFreeBufferLength(), channel_capacity - 1);
        EXPECT_EQ(channel.GetQueuedBufferLength(), 0);
    }
    //Then
    EXPECT_EQ(channel.GetFreeBufferLength(), channel_capacity - 1);
    EXPECT_EQ(channel.GetQueuedBufferLength(), 1);
}

TEST_F(ScheduledChannelTest, ScheduledChannelCallbackTranslatorTest) {
    // Given
    ::pipert::ScheduledChannel<int> channel = scheduler_.CreateScheduledChannel<int>("TestChannel", 2, nullptr, std::bind(&OtherPrinter::Add, &op_, std::placeholders::_1));
    ::pipert::PacketToFill<int> packet = channel.Acquire("TestChannel", default_time_, 10);
    // When
    channel.CallbackTranslator(&channel, packet.GetPacket());
    // Given
    EXPECT_EQ(packet.data(), 11);
}