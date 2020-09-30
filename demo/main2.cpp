#include "pipert/Channel.h"
#include "pipert/Packet.h"
#include "pipert/Scheduler.h"
#include "pipert/Timer.h"
#include <iostream>

#include <unistd.h>

struct Human {
  std::string name;
  Human(const std::string& name) : name (name) {}

  std::string GetName() const { return name; }
};

class HumanPrinter {
public:
  HumanPrinter(pipert::ScheduledChannel<Human>* ch_to_write) : ch_to_write_(ch_to_write) {}
  
  void Print(pipert::PacketToProcess<Human> packet) {
    std::cout << packet.data().GetName() << std::endl;
    ch_to_write_->Acquire("Other", pipert::Timer::time(), packet.data());
  }

private:
  pipert::ScheduledChannel<Human>* ch_to_write_;
};

class OtherPrinter {
public:
  void Print(pipert::PacketToProcess<Human> packet) {
    std::cout << "Other: " << packet.data().GetName() << std::endl;
  }
};

int main() {
  // Scheduler created
  pipert::Scheduler sch(4);
  OtherPrinter op;
  pipert::ScheduledChannel<Human> sc2 = sch.CreateScheduledChannel<Human>("Other", 2, nullptr, std::bind(&OtherPrinter::Print, &op, std::placeholders::_1));

  HumanPrinter hp(&sc2);

  pipert::ScheduledChannel<Human> sc1 = sch.CreateScheduledChannel<Human>("Print", 2, nullptr, std::bind(&HumanPrinter::Print, &hp, std::placeholders::_1));

  //auto packet2 = pc.Acquire("Network", pipert::Timer::time(), "Kurt Cobain");
  
  //sc.Push(&packet);

  /*pc.Push(&packet2);
  auto process = pc.Poll();
  std::cout << process.data().GetName() << std::endl;
  */

  //sc.Execute();

  // Starting the scheduler
  sch.Start();


  //sc.Push(&packet);
  //auto packetBase = sc.GetNext();

  for (int i = 0; i < 10; ++i) {
    usleep(100);
    auto packet = sc1.Acquire("Print", pipert::Timer::time(), "Jimmy Hendrix" + std::to_string(i));
  }
   
  usleep(100000);
  std::cout << "Program will end soon" << std::endl;
  // Stopping the scheduler
  sch.Stop();
  return 0;
}