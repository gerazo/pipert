#include "gtest/gtest.h"
#include "pipert/Timer.h"

#include <thread>

TEST(TimerTest, CurrentTimeIsOK) {
  pipert::Timer::Time t1 = pipert::Timer::time();
  std::this_thread::sleep_for(std::chrono::microseconds(3));
  pipert::Timer::Time t2 = pipert::Timer::time();
  auto d = t2 - t1;
  EXPECT_GT(d, 0);
}

TEST(TimerTest, CurrentTimeCanBeReadRapidly) {
  pipert::Timer::Time t1 = pipert::Timer::time();
  pipert::Timer::Time t2 = pipert::Timer::time();
  auto d = t2 - t1;
  EXPECT_GE(d, 0);
}

TEST(TimerTest, MeasurementInmsecIsOK) {
  pipert::Timer t;
  t.tic();
  std::this_thread::sleep_for(std::chrono::milliseconds(3));
  EXPECT_GT(t.toc(), 0.0);
}

TEST(TimerTest, MeasurementInusecIsOK) {
  pipert::Timer t;
  t.tic();
  std::this_thread::sleep_for(std::chrono::microseconds(3));
  EXPECT_GT(t.toc_in_us(), 0);
}