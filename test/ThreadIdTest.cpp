#include "gtest/gtest.h"
#include "ThreadId.h"

#include <thread>

TEST(ThreadIdTest, SerializableDataIsConsistent) {
  pipert::ThreadId id = pipert::ThreadId::GetCurrentThread();
  auto val1 = id.GetIdForSerialization();
  EXPECT_NE(val1, 0);
  auto val2 = id.GetIdForSerialization();
  EXPECT_NE(val2, 0);
  EXPECT_EQ(val1, val2);
}

TEST(ThreadIdTest, ShouldBeHandOrAutoTagged) {
  pipert::ThreadId id = pipert::ThreadId::GetCurrentThread();
  EXPECT_TRUE(id.IsHandTagged() || id.IsAutoTagged());
}

void HandTaggedThread() {
  EXPECT_TRUE(pipert::ThreadId::IsCurrentThreadUntagged());
  pipert::ThreadId::TagCurrentThread();
  EXPECT_FALSE(pipert::ThreadId::IsCurrentThreadUntagged());
  pipert::ThreadId id = pipert::ThreadId::GetCurrentThread();
  EXPECT_FALSE(pipert::ThreadId::IsCurrentThreadUntagged());
  EXPECT_TRUE(id.IsHandTagged());
}

TEST(ThreadIdTest, CreateHandTaggedThread) {
  std::thread t(HandTaggedThread);
  t.join();
}

void AutoTaggedThread() {
  EXPECT_TRUE(pipert::ThreadId::IsCurrentThreadUntagged());
  pipert::ThreadId id = pipert::ThreadId::GetCurrentThread();
  EXPECT_FALSE(pipert::ThreadId::IsCurrentThreadUntagged());
  EXPECT_TRUE(id.IsAutoTagged());
}

TEST(ThreadIdTest, CreateAutoTaggedThread) {
  std::thread t(AutoTaggedThread);
  t.join();
}

const int thread_num = 4;
int_least32_t tags[thread_num];

void TagExportingThread(int i) {
  EXPECT_TRUE(pipert::ThreadId::IsCurrentThreadUntagged());
  pipert::ThreadId::TagCurrentThread();
  EXPECT_FALSE(pipert::ThreadId::IsCurrentThreadUntagged());
  pipert::ThreadId id = pipert::ThreadId::GetCurrentThread();
  EXPECT_TRUE(id.IsHandTagged());
  int_least32_t val = id.GetIdForSerialization();
  tags[i] = val;
}

TEST(ThreadIdTest, MultipleThreadsHaveDifferentID) {
  std::thread t[thread_num];
  for (int i = 0; i < thread_num; i++) tags[i] = 0;
  for (int i = 0; i < thread_num; i++)
    t[i] = std::thread(TagExportingThread, i);
  for (int i = 0; i < thread_num; i++) t[i].join();
  for (int i = 0; i < thread_num - 1; i++)
    for (int j = i + 1; j < thread_num; j++) EXPECT_NE(tags[i], tags[j]);
}