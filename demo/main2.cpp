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
  void Print(pipert::PacketToProcess<Human> packet) {
    std::cout << packet.data().GetName() << std::endl;
  }
};

int main() {
  // Scheduler created
  pipert::Scheduler sch(4);
  HumanPrinter hp;

  pipert::ScheduledChannel<Human> sc = sch.CreateScheduledChannel<Human>("Print", 2, &hp, std::bind(&HumanPrinter::Print, &hp, std::placeholders::_1));

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
    auto packet = sc.Acquire("Print", pipert::Timer::time(), "Jimmy Hendrix" + std::to_string(i));
    usleep(30);
  }
  usleep(300);

  // Stopping the scheduler
  sch.Stop();
  return 0;
}