#include <arpa/inet.h>

#include <thread>

#include "gtest/gtest.h"
#include "pipert/Scheduler.h"

namespace {
const std::uint16_t server_port = 8888;
const int buffer_size = 131;
std::uint8_t buffer[buffer_size];
int sockfd;

void PrepareToReceive() {
  struct sockaddr_in servaddr;

  // Creating socket file descriptor
  if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
    ASSERT_TRUE(false);  // socket creation failed
  }

  memset(&servaddr, 0, sizeof(servaddr));

  // Filling server information
  servaddr.sin_family = AF_INET;  // IPv4
  servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
  servaddr.sin_port = htons(server_port);

  // Bind the socket with the server address
  if (bind(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
    ASSERT_TRUE(false);  // bind failed
  }
}

bool Receive() {
  struct sockaddr_in cliaddr;
  memset(&cliaddr, 0, sizeof(cliaddr));
  socklen_t len = sizeof(cliaddr);
  int retbuflen = recvfrom(sockfd, buffer, buffer_size, 0,
                           (struct sockaddr *)&cliaddr, &len);
  if (retbuflen != buffer_size) return false;
  if (buffer[0] == 68U && buffer[37] == 71U && buffer[94] == 115U) {
    close(sockfd);
    return true;
  } else {
    close(sockfd);
    return false;
  }
}

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
    EXPECT_EQ(p.data().GetName(), "Jimi Hendrix");
    EXPECT_TRUE(p.data().GetAge() >= 28 && p.data().GetAge() <= 37);
    std::string name(p.data().GetName());
    std::reverse(name.begin(), name.end());
    pc_to_write_->Acquire(p.timestamp() + 10, name, p.data().GetAge());
  }

 private:
  pipert::PolledChannel<Human> *pc_to_write_;
};

class HumanPrinter {
 public:
  HumanPrinter(pipert::ScheduledChannel<Human> *ch_to_write)
      : ch_to_write_(ch_to_write) {}

  void PrintName(pipert::PacketToProcess<Human> p) {
    EXPECT_EQ(p.data().GetName(), "Jimi Hendrix");
    EXPECT_TRUE(p.data().GetAge() >= 28 && p.data().GetAge() <= 37);
    // std::cout << packet.data().GetName() << std::endl;
    ch_to_write_->Acquire(p.timestamp() + 5, p.data());
  }

 private:
  pipert::ScheduledChannel<Human> *ch_to_write_;
};

TEST(ProfilerTest, EmptyIsHarmless) {
  pipert::Profiler profiler;
  EXPECT_EQ(profiler.GetAggregationTime(), 0);
  EXPECT_EQ(profiler.GetBufferSize(), 0);
  profiler.GatherNSend();  // nothing happens
}

TEST(ProfilerTest, CanBeMoved) {
  pipert::Profiler profiler = pipert::Profiler();
  EXPECT_EQ(profiler.GetAggregationTime(), 0);
  EXPECT_EQ(profiler.GetBufferSize(), 0);
  profiler.GatherNSend();  // nothing happens
}

TEST(ProfilerTest, CreateForFile) {
  {
    pipert::Profiler profiler("file:emptylog.txt");
    EXPECT_EQ(profiler.GetAggregationTime(), 0);
    EXPECT_GT(profiler.GetBufferSize(), 0);
    profiler.GatherNSend();  // data written
  }
  std::FILE *file = std::fopen("emptylog.txt", "rb");
  EXPECT_NE(file, nullptr);
  if (file) {
    EXPECT_EQ(std::fseek(file, 0, SEEK_END), 0);
    EXPECT_EQ(std::ftell(file), 0);
    EXPECT_EQ(std::fclose(file), 0);
  }
  EXPECT_EQ(std::remove("emptylog.txt"), 0);
}

TEST(ProfilerTest, CreateForUDP) {
  pipert::Profiler profiler("udp:127.0.0.1:8888");
  EXPECT_EQ(profiler.GetAggregationTime(), 0);
  EXPECT_GT(profiler.GetBufferSize(), 0);
  EXPECT_LE(profiler.GetBufferSize(), 508);
}

TEST(ProfilerTest, TestUDPUsingSchedular) {
  PrepareToReceive();
  pipert::Scheduler sch(0, pipert::Profiler("udp:127.0.0.1:8888"));
  int channel_capacity = 10;
  pipert::PolledChannel<Human> pc =
      sch.CreatePolledChannel<Human>("OutChannel", channel_capacity);

  HumanReverser hr(&pc);
  pipert::ScheduledChannel<Human> sc2 = sch.CreateScheduledChannel<Human>(
      "ReverserChannel", channel_capacity, nullptr,
      std::bind(&HumanReverser::ReverseName, &hr, std::placeholders::_1));

  HumanPrinter hp(&sc2);
  pipert::ScheduledChannel<Human> sc1 = sch.CreateScheduledChannel<Human>(
      "PrinterChannel", channel_capacity, nullptr,
      std::bind(&HumanPrinter::PrintName, &hp, std::placeholders::_1));

  sch.Start();

  pipert::Timer::Time time = pipert::Timer::time();
  for (int i = 0; i < 10; i++) {
    pipert::PacketToFill<Human> packet_to_fill =
        sc1.Acquire(time, "Jimi Hendrix", 28 + i);
  }

  while (pc.GetQueuedBufferLength() != channel_capacity)
    std::this_thread::yield();

  sch.GetProfiler().GatherNSend();
  EXPECT_EQ(sch.GetProfiler().GetAggregationTime(), 0);
  EXPECT_EQ(sch.GetProfiler().GetBufferSize(), 508);
  sch.Stop();

  for (pipert::PacketToProcess<Human> packet_to_process = pc.Poll();
       !packet_to_process.IsEmpty(); packet_to_process = pc.Poll()) {
    EXPECT_EQ(packet_to_process.data().GetName(), "xirdneH imiJ");
    EXPECT_TRUE(packet_to_process.data().GetAge() >= 28 &&
                packet_to_process.data().GetAge() <= 37);
    EXPECT_EQ(packet_to_process.timestamp(), time + 15);
  }
  // test receiving data
  EXPECT_EQ(Receive(), true);
}
}  // namespace
