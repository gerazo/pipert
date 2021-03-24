#include "gtest/gtest.h"

#include "ProfilerImpl.h"

#include <atomic>
#include <cstdint>
#include <cstring>
#include <thread>

#include "ProfileData.h"
#include "pipert/LogEvent.h"

namespace {

const int kMaxUDPSize = 508;
const int kTenthSecond = 100;
const int kOneMilliSecond = 1;
const int kOneTimeSlice = 10;
const char kDataName[] = "MyData";
const char kDataNameAlt[] = "MyOtherData";

std::atomic<int> call_number(0);

void SenderFunc(std::uint8_t* buffer, int size) {
  EXPECT_TRUE(strncmp((char*)buffer, "DGRP", 4) == 0);
  EXPECT_GT(size, 10);
  call_number++;
}

}  // namespace

TEST(ProfilerImplTest, CreateNDestroy) {
  pipert::ProfilerImpl profiler(&SenderFunc, kMaxUDPSize, kTenthSecond);
  EXPECT_EQ(profiler.GetAggregationTime(), kTenthSecond);
  EXPECT_EQ(profiler.GetBufferSize(), kMaxUDPSize);
}

TEST(ProfilerImplTest, StartNStop) {
  pipert::ProfilerImpl profiler(&SenderFunc, kMaxUDPSize, kTenthSecond);
  profiler.Start();
  EXPECT_EQ(profiler.GetAggregationTime(), kTenthSecond);
  EXPECT_EQ(profiler.GetBufferSize(), kMaxUDPSize);
  profiler.Stop();
}

TEST(ProfilerImplTest, StartWithoutStop) {
  pipert::ProfilerImpl profiler(&SenderFunc, kMaxUDPSize, kTenthSecond);
  profiler.Start();
  EXPECT_EQ(profiler.GetAggregationTime(), kTenthSecond);
  EXPECT_EQ(profiler.GetBufferSize(), kMaxUDPSize);
}

TEST(ProfilerImplTest, StartNStopWithoutBGThread) {
  pipert::ProfilerImpl profiler(&SenderFunc, kMaxUDPSize);
  profiler.Start();
  EXPECT_EQ(profiler.GetAggregationTime(), 0);
  EXPECT_EQ(profiler.GetBufferSize(), kMaxUDPSize);
  profiler.Stop();
}

TEST(ProfilerImplTest, RunEmpty) {
  call_number = 0;
  pipert::ProfilerImpl profiler(&SenderFunc, kMaxUDPSize, kOneMilliSecond);
  profiler.Start();
  int i = 0;
  while (i < 9 && !call_number) {
    std::this_thread::sleep_for(std::chrono::milliseconds(kOneTimeSlice));
    i++;
  }
  profiler.Stop();
  EXPECT_EQ(call_number, 0);
}

TEST(ProfilerImplTest, RunWithEmptyData) {
  call_number = 0;
  pipert::ProfileData prof_data(kDataName);
  pipert::ProfilerImpl profiler(&SenderFunc, kMaxUDPSize, kOneMilliSecond);
  profiler.AddProfileData(&prof_data);
  profiler.Start();
  int i = 0;
  while (i < 99 && !call_number) {
    std::this_thread::sleep_for(std::chrono::milliseconds(kOneTimeSlice));
    i++;
  }
  profiler.Stop();
  profiler.RemoveProfileData(&prof_data);
  EXPECT_GT(call_number, 0);
}

TEST(ProfilerImplTest, RunWithDataBeingFilled) {
  call_number = 0;
  pipert::ProfileData prof_data(kDataName);
  pipert::ProfilerImpl profiler(&SenderFunc, kMaxUDPSize, kOneMilliSecond);
  profiler.AddProfileData(&prof_data);
  profiler.Start();
  for (int i = 0; i < 9; i++) {
    prof_data.Log(pipert::LogEvent<pipert::ProfileData::kEventPushed>(1.0));
    std::this_thread::sleep_for(std::chrono::milliseconds(kOneMilliSecond));
  }
  int i = 0;
  while (i < 99 && !call_number) {
    std::this_thread::sleep_for(std::chrono::milliseconds(kOneTimeSlice));
    i++;
  }
  profiler.Stop();
  profiler.RemoveProfileData(&prof_data);
  EXPECT_GT(call_number, 0);
}

TEST(ProfilerImplTest, RunWithMultipleDataBeingFilled) {
  call_number = 0;
  pipert::ProfileData prof_data1(kDataName);
  pipert::ProfileData prof_data2(kDataNameAlt);
  pipert::ProfilerImpl profiler(&SenderFunc, kMaxUDPSize, kOneMilliSecond);
  profiler.AddProfileData(&prof_data1);
  profiler.AddProfileData(&prof_data2);
  profiler.Start();
  for (int i = 0; i < 9; i++) {
    prof_data1.Log(pipert::LogEvent<pipert::ProfileData::kEventPushed>(1.0));
    prof_data2.Log(pipert::LogEvent<pipert::ProfileData::kEventRetrieved>(1.0));
    std::this_thread::sleep_for(std::chrono::milliseconds(kOneMilliSecond));
  }
  int i = 0;
  while (i < 99 && !call_number) {
    std::this_thread::sleep_for(std::chrono::milliseconds(kOneTimeSlice));
    i++;
  }
  profiler.Stop();
  profiler.RemoveProfileData(&prof_data1);
  profiler.RemoveProfileData(&prof_data2);
  EXPECT_GT(call_number, 0);
}
