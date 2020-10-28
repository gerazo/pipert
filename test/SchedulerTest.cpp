#include "pipert/Scheduler.h"
#include "gtest/gtest.h"

class Human {
 public:
  Human(const std::string& name, int age) : name_(name), age_(age) {}

  std::string GetName() const { return name_; }
  int GetAge() const { return age_; }

 private:
  std::string name_;
  int age_;
};

using SchedulerDeathTest = pipert::Scheduler;

template <class T>
class SchedulerTest : public testing::Test {};

using MyTypes = ::testing::Types<char, int, Human>;
TYPED_TEST_SUITE(SchedulerTest, MyTypes, );

void SchedulerInitTestWithParameter(int workers) {
  pipert::Scheduler sch(workers);
  exit(0);
}

void DoNothing() { return; }

TEST(SchedulerDeathTest, InitializationWithParameter) {
#ifdef DEBUG
  EXPECT_DEATH(SchedulerInitTestWithParameter(-5), ".*");
  EXPECT_DEATH(SchedulerInitTestWithParameter(-1), ".*");
#endif  // DEBUG

  EXPECT_EXIT(SchedulerInitTestWithParameter(0), ::testing::ExitedWithCode(0),
              ".*");
  EXPECT_EXIT(SchedulerInitTestWithParameter(1), ::testing::ExitedWithCode(0),
              ".*");
  EXPECT_EXIT(SchedulerInitTestWithParameter(20), ::testing::ExitedWithCode(0),
              ".*");
}

TEST(SchedulerDeathTest, InitializationWithoutParameter) {
  EXPECT_EXIT(
      {
        pipert::Scheduler sch;
        exit(0);
      },
      ::testing::ExitedWithCode(0), ".*");
}

TEST(SchedulerDeathTest, SchedulerDoubleStart) {
  testing::FLAGS_gtest_death_test_style = "threadsafe";
  pipert::Scheduler sch;

  sch.Start();
  EXPECT_DEBUG_DEATH(sch.Start(), ".*");
  sch.Stop();
}

TEST(SchedulerDeathTest, StopNotRunningScheduler) {
  pipert::Scheduler sch;

  EXPECT_DEBUG_DEATH(sch.Stop(), ".*");
}

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
