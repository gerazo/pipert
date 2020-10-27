#include "pipert/Scheduler.h"
#include "gtest/gtest.h"

using SchedulerDeathTest = pipert::Scheduler;

void SchedulerInitTestWithParameter(int workers) {
  pipert::Scheduler sch(workers);
  exit (0);
}

TEST(SchedulerDeathTest, InitializationWithParameter) {
#ifdef DEBUG
  EXPECT_DEATH(SchedulerInitTestWithParameter(-5), ".*");
  EXPECT_DEATH(SchedulerInitTestWithParameter(-1), ".*");
#endif //DEBUG

  EXPECT_EXIT(SchedulerInitTestWithParameter(0), ::testing::ExitedWithCode(0), ".*");
  EXPECT_EXIT(SchedulerInitTestWithParameter(1), ::testing::ExitedWithCode(0), ".*");
  EXPECT_EXIT(SchedulerInitTestWithParameter(20), ::testing::ExitedWithCode(0), ".*");
}

TEST(SchedulerDeathTest, InitializationWithoutParameter) {
  EXPECT_EXIT({pipert::Scheduler sch; exit (0);}, ::testing::ExitedWithCode(0), ".*");
}

TEST(SchedulerDeathTest, SchedulerDoubleStart) {
  testing::FLAGS_gtest_death_test_style="threadsafe";
  pipert::Scheduler sch;

  sch.Start();
  EXPECT_DEBUG_DEATH(sch.Start(), ".*");
  sch.Stop();
}

TEST(SchedulerDeathTest, StopNotRunningScheduler) {
  pipert::Scheduler sch;

  EXPECT_DEBUG_DEATH(sch.Stop(), ".*");
}
