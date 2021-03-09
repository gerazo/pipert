#include <cstdio>
#include <thread>
#include "gtest/gtest.h"
#include "pipert/Packet.h"
#include "pipert/PolledChannel.h"
#include "pipert/ScheduledChannel.h"
#include "pipert/Scheduler.h"
#include "pipert/Timer.h"

namespace {

const int kBigTimeSlice = 500;
const int kBufferSize = 1024;

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

void GenerateDataToChannel(::pipert::ScheduledChannel<int>* channel) {
  for (int i = 0; i < 100; ++i) {
    ::pipert::PacketToFill<int> packet =
        channel->Acquire(::pipert::Timer::time(), 1);
    while (packet.IsEmpty()) {
      packet = channel->Acquire(::pipert::Timer::time(), 1);
    }
    packet.Push();
  }
}

template <typename T>
T ConvertNetworkToHostByteOrder(T val) {
  T ret = 0;
  std::uint8_t* bytes = (std::uint8_t*)&val;
  for (int i = 0; i < (int)sizeof(T); i++) {
    ret = ((ret << 8) | bytes[i]);
  }
  return ret;
}

}  // namespace

TEST(StateScheduling, SchedulingWithoutStateTest) {
  ::pipert::Scheduler sch(4);
  Processor processor;
  ::pipert::ScheduledChannel<int> incrementer = sch.CreateScheduledChannel<int>(
      "Incrementer", 4, nullptr,
      std::bind(&Processor::Process, &processor, std::placeholders::_1));

  sch.Start();
  // This intentionally creates data race, so a checker could be angry...
  for (int i = 0; i < 100; ++i) {
    ::pipert::PacketToFill<int> packet =
        incrementer.Acquire(::pipert::Timer::time(), 1);
  }

  sch.Stop();
  EXPECT_TRUE(-1000 <= processor.GetValue() && processor.GetValue() <= 1000);
}

TEST(StateScheduling, SchedulingWithStateTest) {
  ::pipert::Scheduler sch(4);
  Processor processor;
  ::pipert::ScheduledChannel<int> incrementer = sch.CreateScheduledChannel<int>(
      "Processor", 4, &processor,
      std::bind(&Processor::Process, &processor, std::placeholders::_1));

  sch.Start();
  for (int i = 0; i < 100; ++i) {
    ::pipert::PacketToFill<int> packet =
        incrementer.Acquire(::pipert::Timer::time(), 1);
  }

  sch.Stop();
  EXPECT_EQ(processor.GetValue(), 0);
}

TEST(StateScheduling, SchedulingDropCheckTest) {
  Merger incrementor;
  char file_URI[L_tmpnam + 8] = "file:";
  char* temp_file = file_URI + strlen(file_URI);
  std::tmpnam(temp_file);
  {
    ::pipert::Scheduler sch(4, ::pipert::Profiler(file_URI));
    ::pipert::ScheduledChannel<int> incChannel =
        sch.CreateScheduledChannel<int>(
            "Incrementor", 4, &incrementor,
            std::bind(&Merger::Increment, &incrementor, std::placeholders::_1));
    sch.Start();
    std::thread incThread(GenerateDataToChannel, &incChannel);
    incThread.join();
    std::this_thread::sleep_for(std::chrono::milliseconds(kBigTimeSlice));
    sch.Stop();
    sch.GetProfiler().GatherNSend();
  }
  std::FILE* f = std::fopen(temp_file, "rb");
  char buf[kBufferSize];
  int len = (int)std::fread(buf, 1, kBufferSize, f);
  ASSERT_TRUE(len <= kBufferSize);
  std::fclose(f);
  std::remove(temp_file);

  char* p = buf;
  do {
    p = p + strlen(p) + 1;
  } while (p - buf < len && !strstr(p, "LOGAPacket Dropped"));
  int dropped_packets = (p - buf < len) ? *(int*)(p + strlen(p) + 1) : 0;
  dropped_packets = ConvertNetworkToHostByteOrder(dropped_packets);

  EXPECT_EQ(incrementor.GetValue() + dropped_packets, 100);
}

TEST(StateScheduling, MergeSchedulingTest) {
  Merger merger;
  char file_URI[L_tmpnam + 8] = "file:";
  char* temp_file = file_URI + strlen(file_URI);
  std::tmpnam(temp_file);
  {
    ::pipert::Scheduler sch(4, ::pipert::Profiler(file_URI));
    ::pipert::ScheduledChannel<int> incChannel =
        sch.CreateScheduledChannel<int>(
            "Incrementor", 4, &merger,
            std::bind(&Merger::Increment, &merger, std::placeholders::_1));
    ::pipert::ScheduledChannel<int> decChannel =
        sch.CreateScheduledChannel<int>(
            "Decrementor", 4, &merger,
            std::bind(&Merger::Decrement, &merger, std::placeholders::_1));
    sch.Start();
    std::thread incThread(GenerateDataToChannel, &incChannel);
    std::thread decThread(GenerateDataToChannel, &decChannel);
    incThread.join();
    decThread.join();
    std::this_thread::sleep_for(std::chrono::milliseconds(kBigTimeSlice));
    sch.Stop();
    sch.GetProfiler().GatherNSend();
  }
  std::FILE* f = std::fopen(temp_file, "rb");
  char buf[kBufferSize];
  int len = (int)std::fread(buf, 1, kBufferSize, f);
  ASSERT_TRUE(len <= kBufferSize);
  std::fclose(f);
  std::remove(temp_file);

  char* p = buf;
  while (p - buf < len && !strstr(p, "DGRPIncrementor")) {
    p = p + strlen(p) + 1;
  }
  ASSERT_TRUE(p - buf < len);
  do {
    p = p + strlen(p) + 1;
  } while (p - buf < len && !strstr(p, "LOGAPacket Dropped"));
  int dropped_packets1 = (p - buf < len) ? *(int*)(p + strlen(p) + 1) : 0;
  dropped_packets1 = ConvertNetworkToHostByteOrder(dropped_packets1);

  p = buf;
  while (p - buf < len && !strstr(p, "DGRPDecrementor")) {
    p = p + strlen(p) + 1;
  }
  ASSERT_TRUE(p - buf < len);
  do {
    p = p + strlen(p) + 1;
  } while (p - buf < len && !strstr(p, "LOGAPacket Dropped"));
  int dropped_packets2 = (p - buf < len) ? *(int*)(p + strlen(p) + 1) : 0;
  dropped_packets2 = ConvertNetworkToHostByteOrder(dropped_packets2);

  EXPECT_EQ(merger.GetValue() + dropped_packets1 - dropped_packets2, 0);
}
