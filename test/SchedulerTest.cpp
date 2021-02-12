#include "pipert/Scheduler.h"
#include "pipert/Profiler.h"
#include "gtest/gtest.h"

#include <thread>
#include <string>

namespace {

class Human {
 public:
  Human(const std::string& name, int age) : name_(name), age_(age) {}

  std::string GetName() const { return name_; }
  int GetAge() const { return age_; }

 private:
  std::string name_;
  int age_;
};

class HumanReverser {
 public:
  HumanReverser(pipert::PolledChannel<Human>* pc_to_write) : pc_to_write_(pc_to_write) {}

  void ReverseName(pipert::PacketToProcess<Human> p) {
    EXPECT_EQ(p.data().GetName(), "Jimi Hendrix");
    EXPECT_TRUE(p.data().GetAge() >= 28 &&
                p.data().GetAge() <= 37);
    std::string name(p.data().GetName());
    std::reverse(name.begin(), name.end());
    pc_to_write_->Acquire(p.timestamp() + 10, name, p.data().GetAge());
  }

 private:
  pipert::PolledChannel<Human>* pc_to_write_;
};

class HumanPrinter {
 public:
  HumanPrinter(pipert::ScheduledChannel<Human>* ch_to_write) : ch_to_write_(ch_to_write) {}

  void PrintName(pipert::PacketToProcess<Human> p) {
    EXPECT_EQ(p.data().GetName(), "Jimi Hendrix");
    EXPECT_TRUE(p.data().GetAge() >= 28 &&
                p.data().GetAge() <= 37);
    // std::cout << packet.data().GetName() << std::endl;
    ch_to_write_->Acquire(p.timestamp() + 5, p.data());
  }

 private:
  pipert::ScheduledChannel<Human>* ch_to_write_;
};

template <class T>
class SchedulerTest : public testing::Test {};

using MyTypes = ::testing::Types<char, int, Human>;
TYPED_TEST_SUITE(SchedulerTest, MyTypes, );

void DoNothing() { return; }

}  // namespace

TEST(Scheduler, SchedulerInitializationTest) {
  pipert::Scheduler sch0;
  pipert::Scheduler sch1(-1);
  pipert::Scheduler sch2(0);
  pipert::Scheduler sch3(1);

  EXPECT_EQ(sch0.GetWorkerNumber(), std::thread::hardware_concurrency());
  EXPECT_EQ(sch1.GetWorkerNumber(), std::thread::hardware_concurrency());
  EXPECT_EQ(sch2.GetWorkerNumber(), std::thread::hardware_concurrency());
  EXPECT_EQ(sch3.GetWorkerNumber(), 1);
}

TEST(Scheduler, StartStopRestartTest) {
  pipert::Scheduler sch;

  EXPECT_FALSE(sch.IsRunning());
  sch.Start();
  EXPECT_TRUE(sch.IsRunning());
  sch.Stop();
  EXPECT_FALSE(sch.IsRunning());
  sch.Start();
  EXPECT_TRUE(sch.IsRunning());
  sch.Stop();
  EXPECT_FALSE(sch.IsRunning());
}

TYPED_TEST(SchedulerTest, PolledChannelCreationWithTypes) {
  pipert::Scheduler sch(0);
  pipert::PolledChannel<TypeParam> pc =
      sch.CreatePolledChannel<TypeParam>("PolledChannel", 1);

  EXPECT_EQ(pc.GetName(), "PolledChannel");
  EXPECT_EQ(pc.GetCapacity(), 1);
  EXPECT_EQ(pc.GetPacketSize(), sizeof(pipert::Packet<TypeParam>));
}

TYPED_TEST(SchedulerTest, ScheduledChannelCreationWithTypes) {
  pipert::Scheduler sch(0);
  pipert::ScheduledChannel<TypeParam> sc =
      sch.CreateScheduledChannel<TypeParam>("ScheduledChannel", 1, nullptr,
                                            std::bind(&DoNothing));

  EXPECT_EQ(sc.GetName(), "ScheduledChannel");
  EXPECT_EQ(sc.GetCapacity(), 1);
  EXPECT_EQ(sc.GetPacketSize(), sizeof(pipert::Packet<TypeParam>));
}

TEST(Scheduler, SchedulerPipeLineTest) {
  pipert::Scheduler sch(0, pipert::Profiler("file:profilerlog.txt"));
  int channel_capacity = 10;
  pipert::PolledChannel<Human> pc =
      sch.CreatePolledChannel<Human>("OutChannel", channel_capacity);

  HumanReverser hr(&pc);
  pipert::ScheduledChannel<Human> sc2 =
      sch.CreateScheduledChannel<Human>("ReverserChannel", channel_capacity, nullptr,
                                        std::bind(&HumanReverser::ReverseName, &hr, std::placeholders::_1));

  HumanPrinter hp(&sc2);
  pipert::ScheduledChannel<Human> sc1 =
      sch.CreateScheduledChannel<Human>("PrinterChannel", channel_capacity, nullptr,
                                        std::bind(&HumanPrinter::PrintName, &hp, std::placeholders::_1));

  sch.Start();

  pipert::Timer::Time time = pipert::Timer::time();
  for(int i = 0; i < 10; i++) {
    pipert::PacketToFill<Human> packet_to_fill = sc1.Acquire(time, "Jimi Hendrix", 28 + i);
  }

  while(pc.GetQueuedBufferLength() != channel_capacity)
    std::this_thread::yield();

  sch.GetProfiler().GatherNSend();
  EXPECT_EQ(sch.GetProfiler().GetAggregationTime(), 0);
  EXPECT_EQ(sch.GetProfiler().GetBufferSize(), 4096);
  sch.Stop();

  for(pipert::PacketToProcess<Human> packet_to_process = pc.Poll();
      !packet_to_process.IsEmpty();
      packet_to_process = pc.Poll()) {
    EXPECT_EQ(packet_to_process.data().GetName(), "xirdneH imiJ");
    EXPECT_TRUE(packet_to_process.data().GetAge() >= 28 &&
                packet_to_process.data().GetAge() <= 37);
    EXPECT_EQ(packet_to_process.timestamp(), time + 15);
  }
}
