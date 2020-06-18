// g++ join.cpp Timer.cpp SchedulerImp.cpp Scheduler.cpp ChannelBase.cpp
// -pthread

#include <unistd.h>
#include <algorithm>
#include <fstream>
#include <functional>
#include <iostream>
#include <string>
#include "Channel.h"
#include "JoinChannel.h"
#include "Packet.h"
#include "Scheduler.h"

template <class T>
void printPacketTimes(pipert::Packet<T> packet) {
  std::cout << "created: " << packet.GetCreatedTime() << std::endl;
  std::cout << "current: " << packet.GetCurrentTime() << std::endl;
}

class Human {
 public:
  Human(const std::string& firstName, const std::string& lastName, int id)
      : firstName(firstName), lastName(lastName), id(id) {}

  std::string getFirstName() const { return firstName; }

  std::string getLastName() const { return lastName; }

  int getId() const { return id; }

 private:
  int id;
  std::string firstName;
  std::string lastName;
};

class HumanPipe1 {
 public:
  HumanPipe1(pipert::JoinChannel<Human, Human> ch) : ch(ch) {}

  void sendHuman(pipert::Packet<Human> h) {
    usleep(60);
    ch.WriteFst(h);
  }

 private:
  pipert::JoinChannel<Human, Human> ch;
};

class HumanPipe2 {
 public:
  HumanPipe2(pipert::JoinChannel<Human, Human> ch) : ch(ch) {}
  void sendHuman(pipert::Packet<Human> h) {
    usleep(50);
    ch.WriteSnd(h);
  }

 private:
  pipert::JoinChannel<Human, Human> ch;
};

class HumanMerge {
 public:
  void mergeHumans(pipert::Packet<Human> h1, pipert::Packet<Human> h2) {
    usleep(121);
    std::cout << "******* HumanMerge was called by channel *******"
              << std::endl;
    std::cout << h1.GetData().getId() << ":" << h1.GetData().getFirstName()
              << " " << h1.GetData().getLastName();
    std::cout << " :: ";
    std::cout << h2.GetData().getId() << ":" << h2.GetData().getFirstName()
              << " " << h2.GetData().getLastName();
    std::cout << std::endl;
    // std::ofstream out("output.txt", std::ios_base::app);
    // std::string o = std::to_string(h.GetData().getId()) + " : " +
    // h.GetData().getFirstName() + " " + h.GetData().getLastName() + "\n"; out
    // << o; out.close();

    // std::cout << h.GetDataConstPtr()->getId() << " : " << fstn << " " << sndn
    // << std::endl;
  }
};

int main() {
  // Scheduler created
  pipert::Scheduler sch(4);

  // Starting the scheduler
  sch.Start();

  HumanMerge hm;
  pipert::JoinChannel<Human, Human> hmc = sch.MakeJoinChannel<Human, Human>(
      &hm, "HumanEnd",
      std::bind(&HumanMerge::mergeHumans, &hm, std::placeholders::_1,
                std::placeholders::_2),
      4);

  HumanPipe1 hp1(hmc);
  HumanPipe2 hp2(hmc);

  for (int i = 0; i < 30; ++i) {
    Human h("Jimi", "Hendrix", i);
    pipert::Packet<Human> ph(h);
    if (i % 2 == 0)
      hp1.sendHuman(ph);
    else
      hp2.sendHuman(ph);

    usleep(2000);
  }

  usleep(2000);

  // Stopping the scheduler
  sch.Stop();
  return 0;
}