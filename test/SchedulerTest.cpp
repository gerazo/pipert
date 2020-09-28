#include "pipert/Scheduler.h"
#include "gtest/gtest.h"

#include <iostream>
#include <utility>

class Human {
public:
  Human(const std::string& name, int age) : name_(name), age_(age) {}

  std::string GetName() const { return name_; }
  int GetAge() const { return age_; }

private: 
  std::string name_;
  int age_;
};

class Printer {
public:
  void Print(pipert::PacketToProcess<Human> packet) {
    std::cout << packet.data().GetName() << " - " <<  packet.data().GetAge() << std::endl;
  }
};

/*
TEST(SchedulerTest, SchedulerState) {
    pipert::Scheduler sch1(0);
    pipert::Scheduler sch2(2);

    EXPECT_EQ(sch1.IsRunning(), false);
    EXPECT_EQ(sch2.IsRunning(), false);

    sch1.Start();
    EXPECT_EQ(sch1.IsRunning(), true);
    EXPECT_EQ(sch2.IsRunning(), false);

    sch1.Stop();
    sch2.Start();
    EXPECT_EQ(sch1.IsRunning(), false);
    EXPECT_EQ(sch2.IsRunning(), true);

    sch1.Start();
    sch2.Start();
    EXPECT_EQ(sch1.IsRunning(), true);
    EXPECT_EQ(sch2.IsRunning(), true);

    sch1.Stop();
    sch2.Stop();
    EXPECT_EQ(sch1.IsRunning(), false);
    EXPECT_EQ(sch2.IsRunning(), false);
}
*/
/*
TEST(SchedulerTest, ChannelCreation) {
    pipert::Scheduler sch(1);
    pipert::PolledChannel<std::pair<Human, Human> > pc =
          sch.CreatePolledChannel<std::pair<Human, Human> >("PolledChannel", 1);

    EXPECT_EQ(pc.GetCapacity(), 1);
    EXPECT_EQ(pc.GetPacketSize(), sizeof(pipert::Packet<std::pair<Human, Human> >));
    EXPECT_EQ(pc.GetName(), "PolledChannel");

    Printer p;
    pipert::ScheduledChannel<Human> sc = sch.CreateScheduledChannel<Human>("ScheduledChannel", 2, nullptr, std::bind(&Printer::Print, &p, std::placeholders::_1));

    EXPECT_EQ(sc.GetCapacity(), 2);
    EXPECT_EQ(sc.GetPacketSize(), sizeof(pipert::Packet<Human>));
    EXPECT_EQ(sc.GetName(), "ScheduledChannel");
}
*/