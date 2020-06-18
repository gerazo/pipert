#include "PacketImp.h"
#include "Timer.h"
#include "gtest/gtest.h"

struct Human {
  std::string name_;
  int age_;

  Human(const std::string& name, int age) : name_(name), age_(age) {}
};

TEST(PacketImpTest, InitializationWithData) {
  pipert::PacketImp<Human> packet_(Human("Jimi Hendrix", 28));
  EXPECT_EQ(packet_.GetData()->name_, "Jimi Hendrix");
  EXPECT_EQ(packet_.GetData()->age_, 28);
}

TEST(PacketImpTest, InitializationWithCreatedAtTime) {
  auto now = pipert::Timer::time();
  pipert::PacketImp<Human> packet_(Human("Jimi Hendrix", 28), now);
  EXPECT_EQ(packet_.GetData()->name_, "Jimi Hendrix");
  EXPECT_EQ(packet_.GetData()->age_, 28);
  EXPECT_EQ(packet_.GetCreatedTime(), now);
  EXPECT_EQ(packet_.GetCurrentTime(), now);
}

TEST(PacketImpTest, InitilaizationWithOtherPacket) {
  pipert::PacketImp<Human> packet_(Human("Jimi Hendrix", 28));
  pipert::PacketImp<Human> packet2_(Human("Kurt Cobain", 27), &packet_);
  EXPECT_EQ(packet_.GetData()->name_, "Jimi Hendrix");
  EXPECT_EQ(packet_.GetData()->age_, 28);
  EXPECT_EQ(packet2_.GetData()->name_, "Kurt Cobain");
  EXPECT_EQ(packet2_.GetData()->age_, 27);
  EXPECT_EQ(packet_.GetCreatedTime(), packet2_.GetCreatedTime());
  EXPECT_NE(packet_.GetCurrentTime(), packet2_.GetCurrentTime());
}

TEST(PacketImpTest, ChangeDataInPacket) {
  pipert::PacketImp<Human> packet_(Human("Jimi Hendrix", 28));
  EXPECT_EQ(packet_.GetData()->name_, "Jimi Hendrix");
  EXPECT_EQ(packet_.GetData()->age_, 28);
  packet_.GetData()->name_ = "Kurt Cobain";
  packet_.GetData()->age_ = 27;
  EXPECT_EQ(packet_.GetData()->name_, "Kurt Cobain");
  EXPECT_EQ(packet_.GetData()->age_, 27);
}

TEST(PacketImpTest, ChangeCreatedTime) {
  pipert::PacketImp<Human> packet_(Human("Jimi Hendrix", 28));
  EXPECT_EQ(packet_.GetData()->name_, "Jimi Hendrix");
  EXPECT_EQ(packet_.GetData()->age_, 28);
  auto now = pipert::Timer::time();
  auto old = packet_.GetCreatedTime();
  packet_.SetCreatedTime(now);
  EXPECT_EQ(packet_.GetCreatedTime(), now);
  EXPECT_EQ(packet_.GetCurrentTime(), now);
  EXPECT_NE(packet_.GetCreatedTime(), old);
  EXPECT_NE(packet_.GetCurrentTime(), old);
}