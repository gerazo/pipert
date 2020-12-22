#include "gtest/gtest.h"

#include "pipert/Profiler.h"

#include <cstdio>

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
  std::FILE* file = std::fopen("emptylog.txt", "rb");
  EXPECT_NE(file, nullptr);
  if (file) {
    EXPECT_EQ(std::fseek(file, 0, SEEK_END), 0);
    EXPECT_EQ(std::ftell(file), 0);
    EXPECT_EQ(std::fclose(file), 0);
  }
  EXPECT_EQ(std::remove("emptylog.txt"), 0);
}

TEST(ProfilerTest, CreateForUDP) {
  // TODO Prepare UDP dummy receiver
  {
    pipert::Profiler profiler("udp:localhost:9000");
    EXPECT_EQ(profiler.GetAggregationTime(), 0);
    EXPECT_GT(profiler.GetBufferSize(), 0);
    EXPECT_LE(profiler.GetBufferSize(), 508);
    profiler.GatherNSend();  // data sent
  }
  // Check receiveed data
}

// TODO Tests using Scheduler
