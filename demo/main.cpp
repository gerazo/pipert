// g++ main.cpp Timer.cpp SchedulerImp.cpp Scheduler.cpp ChannelBase.cpp
// -pthread

#include <unistd.h>
#include <algorithm>
#include <fstream>
#include <functional>
#include <iostream>
#include <string>
#include "pipert/Channel.h"
#include "pipert/Packet.h"
#include "pipert/Scheduler.h"

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

class HumanPrinter {
 public:
  HumanPrinter(pipert::Scheduler& sch, pipert::Channel<Human> reverseChannel)
      : ch(sch.MakeChannel<Human>(
            "HumanPrinter",
            std::bind(&HumanPrinter::printHuman, this, std::placeholders::_1),
            2)),
        reverseChannel(reverseChannel) {}

  void printHuman(pipert::Packet<Human> h) {
    // std::cout << "******* Printer was called by channel *******" <<
    // std::endl; std::cout << h.GetDataConstPtr()->getId() << " : " <<
    // h.GetDataConstPtr()->getFirstName() << " " <<
    // h.GetDataConstPtr()->getLastName() << std::endl;
    pipert::Packet<Human> newPacket(h.GetData());
    usleep(116);
    reverseChannel.Write(newPacket);
  }

  pipert::Channel<Human> getChannel() { return ch; }

 private:
  pipert::Channel<Human> ch;
  pipert::Channel<Human> reverseChannel;
};

class HumanReverse {
 public:
  HumanReverse(pipert::Channel<Human> ch) : ch(ch) {}
  void printHuman(pipert::Packet<Human> h) {
    // std::cout << "******* Humanreverse was called by channel *******" <<
    // std::endl;
    std::string fstn(h.GetDataPtr()->getFirstName());
    std::string sndn(h.GetDataPtr()->getLastName());
    std::reverse(fstn.begin(), fstn.end());
    std::reverse(sndn.begin(), sndn.end());
    Human newH(fstn, sndn, h.GetData().getId());
    pipert::Packet<Human> hr(newH);
    ch.Write(hr);
    // std::cout << h.GetDataConstPtr()->getId() << " : " << fstn << " " << sndn
    // << std::endl;
  }

 private:
  pipert::Channel<Human> ch;
};

class HumanCount {
 public:
  HumanCount(pipert::Channel<Human> ch) : ch(ch) {}

  void printHuman(pipert::Packet<Human> h) {
    // std::cout << "******* Humanreverse was called by channel *******" <<
    // std::endl; usleep(115);
    std::ofstream out("output.txt", std::ios_base::app);
    std::string o = std::to_string(h.GetData().getId()) + " : " +
                    h.GetData().getFirstName() + " " +
                    h.GetData().getLastName() + "\n";
    out << o;
    out.close();
    // std::cout << h.GetDataConstPtr()->getId() << " : " << fstn << " " << sndn
    // << std::endl;
    ch.Write(h);
  }

 private:
  pipert::Channel<Human> ch;
};

class HumanEnd {
 public:
  void printHuman(pipert::Packet<Human> h) {
    // std::cout << "******* Humanreverse was called by channel *******" <<
    // std::endl;
    usleep(121);
    // std::cout << h.GetDataConstPtr()->getId() << " : " << fstn << " " << sndn
    // << std::endl;
  }
};

int main() {
  // Scheduler created
  pipert::Scheduler sch(4);

  // Starting the scheduler
  sch.Start();

  HumanEnd he;
  pipert::Channel<Human> hec = sch.MakeChannel<Human>(
      &he, "HumanEnd",
      std::bind(&HumanEnd::printHuman, &he, std::placeholders::_1), 4);

  HumanCount hc(hec);
  pipert::Channel<Human> hcc = sch.MakeChannel<Human>(
      &hc, "HumanCount",
      std::bind(&HumanCount::printHuman, &hc, std::placeholders::_1), 4);

  // Create channel for HumanReverse with Node, channel lifetime is bound to
  // main loop
  HumanReverse hr(hcc);
  pipert::Channel<Human> hrc = sch.MakeChannel<Human>(
      &hr, "HumanReverse",
      std::bind(&HumanReverse::printHuman, &hr, std::placeholders::_1), 3);

  // Create HumanPrinter with channel created inside and output channel got in
  // parameter
  HumanPrinter hp(sch, hrc);

  for (int i = 0; i < 100; ++i) {
    Human h("Jimi", "Hendrix", i);
    pipert::Packet<Human> ph(h);
    hp.getChannel().Write(ph);
    usleep(100);
  }

  usleep(2000);

  // Stopping the scheduler
  sch.Stop();
  return 0;
}