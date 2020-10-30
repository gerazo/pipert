#include "pipert/Scheduler.h"
#include "gtest/gtest.h"

#include <string>

class Human {
 public:
  Human(const std::string& name, int age) : name_(name), age_(age) {}

  std::string GetName() const { return name_; }
  int GetAge() const { return age_; }

 private:
  std::string name_;
  int age_;
};

template <class T>
class SchedulerTest : public testing::Test {};

using MyTypes = ::testing::Types<char, int, Human>;
TYPED_TEST_SUITE(SchedulerTest, MyTypes, );

void DoNothing() { return; }

TYPED_TEST(SchedulerTest, PolledChannelCreationWithTypes) {
  pipert::Scheduler sch(0);
  pipert::PolledChannel<TypeParam> pc =
      sch.CreatePolledChannel<TypeParam>("PolledChannel", 1);

  EXPECT_EQ(pc.GetName(), "PolledChannel");
  EXPECT_EQ(pc.GetCapacity(), 1);
  EXPECT_EQ(pc.GetPacketSize(), sizeof(pipert::Packet<TypeParam>));
}

TYPED_TEST(SchedulerTest, ScheduledChannelCreationWithTypes) {
  pipert::Scheduler sch(0);
  pipert::ScheduledChannel<TypeParam> sc =
      sch.CreateScheduledChannel<TypeParam>("ScheduledChannel", 1, nullptr,
                                            std::bind(&DoNothing));

  EXPECT_EQ(sc.GetName(), "ScheduledChannel");
  EXPECT_EQ(sc.GetCapacity(), 1);
  EXPECT_EQ(sc.GetPacketSize(), sizeof(pipert::Packet<TypeParam>));
}
