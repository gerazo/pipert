#include "pipert/Scheduler.h"
#include "pipert/ScheduledChannel.h"
#include "pipert/PolledChannel.h"
#include "pipert/Packet.h"
#include "pipert/Timer.h"
#include "gtest/gtest.h"
#include <iostream>
#include <thread> 

namespace {

class Base {
 public:
  int GetValue() const { return value_; }
 protected:
  int value_ = 0;
};

class Processor : public Base {
 public:
  void Process(::pipert::PacketToProcess<int> packet) {
    for (int i = 0; i < 10; ++i) {
      value_ += packet.data();
    }
    for (int i = 0; i < 10; ++i) {
      value_ -= packet.data();
    }
  }
};

class Merger : public Base {
 public:
  void Increment(::pipert::PacketToProcess<int> packet) {
    value_ += packet.data();
  }

  void Decrement(::pipert::PacketToProcess<int> packet) {
    value_ -= packet.data();
  }
};

void GenerateDataToChannel(::pipert::ScheduledChannel<int>* channel, const char* name) {
  for (int i = 0; i < 100; ++i) {
    ::pipert::PacketToFill<int> packet = channel->Acquire(name, ::pipert::Timer::time(), 1);
    while (packet.IsEmpty()) {
      packet = channel->Acquire(name, ::pipert::Timer::time(), 1);
    }
    packet.Push();
  }
}

}  // namespace

TEST(StateScheduling, SchedulingWithoutStateTest) {
  ::pipert::Scheduler sch(4);
  Processor processor;
  ::pipert::ScheduledChannel<int> incrementer = sch.CreateScheduledChannel<int>("Incrementer", 4, nullptr, std::bind(&Processor::Process, &processor, std::placeholders::_1));

  sch.Start();
  for (int i = 0; i < 100; ++i) {
    ::pipert::PacketToFill<int> packet = incrementer.Acquire("Incrementer", ::pipert::Timer::time(), 1);
  }
   

  sch.Stop();
  std::cout << processor.GetValue() << std::endl;
  EXPECT_TRUE(-1000 <= processor.GetValue() && processor.GetValue() <= 1000);
}

TEST(StateScheduling, SchedulingWithStateTest) {
  ::pipert::Scheduler sch(4);
  Processor processor;
  ::pipert::ScheduledChannel<int> incrementer = sch.CreateScheduledChannel<int>("Processor", 4, &processor, std::bind(&Processor::Process, &processor, std::placeholders::_1));

  sch.Start();
  for (int i = 0; i < 100; ++i) {
    ::pipert::PacketToFill<int> packet = incrementer.Acquire("Processor", ::pipert::Timer::time(), 1);
  }
   
  sch.Stop();
  EXPECT_EQ(processor.GetValue(), 0);
}

TEST(StateScheduling, SchedulingDropCheckTest) {
  ::pipert::Scheduler sch(4);
  Merger incrementor;
  ::pipert::ScheduledChannel<int> incChannel 
            = sch.CreateScheduledChannel<int>("Incrementor", 4, &incrementor, std::bind(&Merger::Increment, &incrementor, std::placeholders::_1));
  sch.Start();
  std::thread incThread(GenerateDataToChannel, &incChannel, "Incrementor");
  incThread.join();
  std::this_thread::sleep_for(std::chrono::seconds(1));
  sch.Stop();
  EXPECT_EQ(incrementor.GetValue() + incChannel.GetDroppedPacketsNumber(), 100);
}

TEST(StateScheduling, MergeSchedulingTest) {
  ::pipert::Scheduler sch(4);
  Merger merger;
  ::pipert::ScheduledChannel<int> incChannel = sch.CreateScheduledChannel<int>("Incrementor", 4, &merger, std::bind(&Merger::Increment, &merger, std::placeholders::_1));
  ::pipert::ScheduledChannel<int> decChannel = sch.CreateScheduledChannel<int>("Decrementor", 4, &merger, std::bind(&Merger::Decrement, &merger, std::placeholders::_1));
  sch.Start();
  std::thread incThread(GenerateDataToChannel, &incChannel, "Incrementor");
  std::thread decThread(GenerateDataToChannel, &decChannel, "Decrementor");
  
  incThread.join();
  decThread.join();
  std::this_thread::sleep_for(std::chrono::seconds(1));
  sch.Stop();

  EXPECT_EQ(merger.GetValue() + incChannel.GetDroppedPacketsNumber() - decChannel.GetDroppedPacketsNumber(), 0);
}