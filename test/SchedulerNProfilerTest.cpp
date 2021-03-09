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
}  // namespace

TEST(Scheduler, SchedulerProfilerPipeLineWithFileTest) {
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
  EXPECT_EQ(std::remove("profilerlog.txt"), 0);
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

TEST(Scheduler, SchedulerProfilerPipeLineWithUDPTest) {
  pipert::Scheduler sch(0, pipert::Profiler("udp:127.0.0.1:8000"));
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
  // EXPECT_EQ(sch.GetProfiler().GetBufferSize(), 4096);
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