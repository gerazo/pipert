#include "pipert/Packet.h"
#include "gtest/gtest.h"

struct Human {
  std::string name_;
  int age_;

  Human(const std::string& name, int age) : name_(name), age_(age) {}
};

TEST(PacketTest, InitializationWithData) {
  auto now = pipert::Timer::time();
  pipert::Packet<Human> packet_(now, "Jimi Hendrix", 28);
  EXPECT_EQ(packet_.data().name_, "Jimi Hendrix");
  EXPECT_EQ(packet_.data().age_, 28);
  EXPECT_EQ(packet_.GetCreatedTime(), now);
}

TEST(PacketTest, InitilaizationWithDifferentTime) {
  auto now = pipert::Timer::time();
  pipert::Packet<Human> packet_(now, "Jimi Hendrix", 28);
  pipert::Packet<Human> packet2_(now + 1, "Kurt Cobain", 27);
  EXPECT_EQ(packet_.data().name_, "Jimi Hendrix");
  EXPECT_EQ(packet_.data().age_, 28);
  EXPECT_EQ(packet2_.data().name_, "Kurt Cobain");
  EXPECT_EQ(packet2_.data().age_, 27);
  EXPECT_NE(packet_.GetCreatedTime(), packet2_.GetCreatedTime());
}

TEST(PacketTest, ChangeDataInPacket) {
  auto now = pipert::Timer::time();
  pipert::Packet<Human> packet_(now, "Jimi Hendrix", 28);
  EXPECT_EQ(packet_.data().name_, "Jimi Hendrix");
  EXPECT_EQ(packet_.data().age_, 28);
  packet_.data().name_ = "Kurt Cobain";
  packet_.data().age_ = 27;
  EXPECT_EQ(packet_.data().name_, "Kurt Cobain");
  EXPECT_EQ(packet_.data().age_, 27);
}
