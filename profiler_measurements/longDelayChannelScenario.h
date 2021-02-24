#ifndef PIPERT_LONGDELAYCHANNELSCENARIO_H
#define PIPERT_LONGDELAYCHANNELSCENARIO_H
#include <fcntl.h>
#include <termio.h>
#include <unistd.h>

#include <algorithm>
#include <iostream>
#include <string>
#include <thread>

#include "pipert/Profiler.h"
#include "pipert/Scheduler.h"

/// Measurement Title : Long Delay Channel detection
/// in this measurement we will generate three channels one of them have
/// relatively longer processing time compared to the average processing  time
/// of the other channels
///
////Expected analysis output
/// log analyser is Expected to activate this flag for the slow channel
// analysis logic is to to compute the average time of channels and activate this flag for the channel which is having noticable higher value

namespace measurements {
class Human {
 public:
  Human(const std::string &name, int age) : name_(name), age_(age) {}

  std::string GetName() const { return name_; }

  int GetAge() const { return age_; }

 private:
  std::string name_;
  int age_;
};

class HumanReverser {
 public:
  HumanReverser(pipert::PolledChannel<Human> *pc_to_write)
      : pc_to_write_(pc_to_write) {}

  void ReverseName(pipert::PacketToProcess<Human> p) {
    std::string name(p.data().GetName());
    std::reverse(name.begin(), name.end());
    pc_to_write_->Acquire(p.timestamp() + 10, name, p.data().GetAge());
  }

 private:
  pipert::PolledChannel<Human> *pc_to_write_;
};

class HumanPrinter {
 public:
  void doNothing() {
    int i = rand() % 99999900 + 1000;

    while (i > 0) {
      i--;
    }
  }

  HumanPrinter(pipert::ScheduledChannel<Human> *ch_to_write)
      : ch_to_write_(ch_to_write) {}

  void PrintName(pipert::PacketToProcess<Human> p) {
    doNothing();
    ch_to_write_->Acquire(p.timestamp() + 5, p.data());
  }

 private:
  pipert::ScheduledChannel<Human> *ch_to_write_;
};

}  // namespace measurements
int kbhit(void) {
  struct termios oldt, newt;
  int ch;
  int oldf;

  tcgetattr(STDIN_FILENO, &oldt);
  newt = oldt;
  newt.c_lflag &= ~(ICANON | ECHO);
  tcsetattr(STDIN_FILENO, TCSANOW, &newt);
  oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
  fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

  ch = getchar();

  tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
  fcntl(STDIN_FILENO, F_SETFL, oldf);

  if (ch != EOF) {
    ungetc(ch, stdin);
    return 1;
  }

  return 0;
}
class longDelayChannelScenario {
 public:
  longDelayChannelScenario() {}

  void run(const char *destination_uri) {
    pipert::Scheduler sch(0, pipert::Profiler(destination_uri));
    while (!kbhit()) {
      int channel_capacity = 10;
      pipert::PolledChannel<measurements::Human> pc =
          sch.CreatePolledChannel<measurements::Human>("OutChannel",
                                                       channel_capacity);

      measurements::HumanReverser hr(&pc);
      pipert::ScheduledChannel<measurements::Human> sc2 =
          sch.CreateScheduledChannel<measurements::Human>(
              "ReverserChannel", channel_capacity, nullptr,
              std::bind(&measurements::HumanReverser::ReverseName, &hr,
                        std::placeholders::_1));

      measurements::HumanPrinter hp(&sc2);
      pipert::ScheduledChannel<measurements::Human> sc1 =
          sch.CreateScheduledChannel<measurements::Human>(
              "PrinterChannel", channel_capacity, nullptr,
              std::bind(&measurements::HumanPrinter::PrintName, &hp,
                        std::placeholders::_1));

      sch.Start();

      pipert::Timer::Time time = pipert::Timer::time();
      for (int i = 0; i < 10; i++) {
        pipert::PacketToFill<measurements::Human> packet_to_fill =
            sc1.Acquire(time, "Jimi Hendrix", 28 + i);
      }

      while (pc.GetQueuedBufferLength() != channel_capacity)
        std::this_thread::yield();
      sch.GetProfiler().GatherNSend();
      sch.Stop();
    }
  }
};
#endif  // PIPERT_LONGDELAYCHANNELSCENARIO_H

