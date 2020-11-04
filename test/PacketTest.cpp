#include "gtest/gtest.h"
#include "pipert/Packet.h"

namespace {

struct Human {
  std::string name_;
  int age_;

  Human(const std::string& name, int age) : name_(name), age_(age) {}
};

}  // namespace

TEST(PacketTest, InitializationWithData) {
  auto now = pipert::Timer::time();
  pipert::Packet<Human> packet_(now, "Jimi Hendrix", 28);
  EXPECT_EQ(packet_.data().name_, "Jimi Hendrix");
  EXPECT_EQ(packet_.data().age_, 28);
  EXPECT_EQ(packet_.timestamp(), now);
}

TEST(PacketTest, InitilaizationWithDifferentTime) {
  auto now = pipert::Timer::time();
  pipert::Packet<Human> packet_(now, "Jimi Hendrix", 28);
  pipert::Packet<Human> packet2_(now + 1, "Kurt Cobain", 27);
  EXPECT_EQ(packet_.data().name_, "Jimi Hendrix");
  EXPECT_EQ(packet_.data().age_, 28);
  EXPECT_EQ(packet2_.data().name_, "Kurt Cobain");
  EXPECT_EQ(packet2_.data().age_, 27);
  EXPECT_NE(packet_.timestamp(), packet2_.timestamp());
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

TEST(PacketTest, HandlingOfRValues) {
  // Given
  auto now = pipert::Timer::time();
  Human a = Human("Hossam", 20);
  // When
  pipert::Packet<Human> packet_(now, a);
  // Then
  EXPECT_EQ(packet_.data().name_, "Hossam");
  EXPECT_EQ(packet_.data().age_, 20);
}

// TEST(PacketTest, WrongInputShouldThrowAnError){
//   // Given
//   auto now = pipert::Timer::time();
//   // When
//   // Then
//   EXPECT_THROW()
// }