#include "pipert/Scheduler.h"
#include "pipert/ScheduledChannel.h"
#include "pipert/PolledChannel.h"
#include "pipert/Packet.h"
#include "pipert/Timer.h"
#include "gtest/gtest.h"
#include <iostream>
#include <unistd.h>

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

class Incrementor : public Base {
 public:
  Incrementor(::pipert::ScheduledChannel<int>&& decChannel) : decChannel_(std::move(decChannel)) {}
  void Increment(::pipert::PacketToProcess<int> packet) {
    value_ += packet.data();
    ::pipert::PacketToFill<int> new_packet = decChannel_.Acquire("Decrementor", packet.timestamp(), packet.data());
    while(packet.IsEmpty()) {
      new_packet = decChannel_.Acquire("Decrementor", packet.timestamp(), packet.data());
    }
    new_packet.Push();
  }

  int GetDroppedPacketsNumber() {
    return decChannel_.GetDroppedPacketsNumber();
  }
 private:
  ::pipert::ScheduledChannel<int> decChannel_;
};

class Decrementor : public Base {
 public:
  void Decrement(::pipert::PacketToProcess<int> packet) {
    value_ -= packet.data();
  }
};

}  // namespace

TEST(StateScheduling, SchedulingWithoutState) {
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

TEST(StateScheduling, SchedulingWithState) {
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

TEST(StateScheduling, SchedulingDropCheck) {
  ::pipert::Scheduler sch(4);
  Decrementor decrementor;
  ::pipert::ScheduledChannel<int> decChannel = sch.CreateScheduledChannel<int>("Decrementor", 4, &decrementor, std::bind(&Decrementor::Decrement, &decrementor, std::placeholders::_1));
  sch.Start();
  for (int i = 0; i < 100; ++i) {
    ::pipert::PacketToFill<int> packet = decChannel.Acquire("Decrementor", ::pipert::Timer::time(), 1);
    while (packet.IsEmpty()) {
      packet = decChannel.Acquire("Decrementor", ::pipert::Timer::time(), 1);
    }
    packet.Push();
  }
  usleep(1000);
  sch.Stop();
  EXPECT_EQ(decrementor.GetValue() - decChannel.GetDroppedPacketsNumber(), -100);
}

TEST(StateScheduling, SchedulingMultipleChannel) {
  ::pipert::Scheduler sch(4);
  Decrementor decrementor;
  Incrementor incrementor(sch.CreateScheduledChannel<int>("Decrementor", 4, &decrementor, std::bind(&Decrementor::Decrement, &decrementor, std::placeholders::_1)));
  ::pipert::ScheduledChannel<int> incChannel = sch.CreateScheduledChannel<int>("Incrementor", 4, &incrementor, std::bind(&Incrementor::Increment, &incrementor, std::placeholders::_1));

  sch.Start();
  for (int i = 0; i < 100; ++i) {
    ::pipert::PacketToFill<int> packet = incChannel.Acquire("Incrementor", ::pipert::Timer::time(), 1);
    while (packet.IsEmpty()) {
      packet = incChannel.Acquire("Incrementor", ::pipert::Timer::time(), 1);
    }
    packet.Push();
  }
   
  usleep(1000);
  sch.Stop();
  EXPECT_EQ(incrementor.GetValue() + incChannel.GetDroppedPacketsNumber(), 100);
  EXPECT_EQ(decrementor.GetValue() - incrementor.GetDroppedPacketsNumber() - incChannel.GetDroppedPacketsNumber(), -100);
}