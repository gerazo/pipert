#include "AdaptiveSpinLock.h"
#include "gtest/gtest.h"

#include <mutex>
#include <thread>

TEST(AdaptiveSpinLockTest, LockUnlockCyclesFinish) {
  pipert::AdaptiveSpinLock lock;
  lock.lock();
  lock.unlock();
  lock.lock();
  lock.unlock();
}

TEST(AdaptiveSpinLockTest, TryLockFailsInLock) {
  pipert::AdaptiveSpinLock lock;
  lock.lock();
  EXPECT_FALSE(lock.try_lock());
  lock.unlock();
}

TEST(AdaptiveSpinLockTest, TryLockStartsLockButFailsSecondTime) {
  pipert::AdaptiveSpinLock lock;
  EXPECT_TRUE(lock.try_lock());
  EXPECT_FALSE(lock.try_lock());
  lock.unlock();
}

TEST(AdaptiveSpinLockTest, RelockingWorks) {
  pipert::AdaptiveSpinLock lock;
  lock.lock();
  EXPECT_FALSE(lock.try_lock());
  lock.unlock();
  EXPECT_TRUE(lock.try_lock());
  EXPECT_FALSE(lock.try_lock());
  lock.unlock();
}

TEST(AdaptiveSpinLockTest, WorksWithStdLockGuard) {
  pipert::AdaptiveSpinLock lock;
  int i = 0;
  {
    std::lock_guard<pipert::AdaptiveSpinLock> guard(lock);
    EXPECT_FALSE(lock.try_lock());
    i = 3;
  }
  EXPECT_EQ(i, 3);
  EXPECT_TRUE(lock.try_lock());
  lock.unlock();
}

TEST(AdaptiveSpinLockTest, WorksWithStdUniqueLock) {
  pipert::AdaptiveSpinLock lock;
  int i = 0;
  {
    std::unique_lock<pipert::AdaptiveSpinLock> ulock(lock);
    EXPECT_FALSE(lock.try_lock());
    i = 3;
  }
  EXPECT_EQ(i, 3);
  EXPECT_TRUE(lock.try_lock());
  lock.unlock();
}

const int thread_num = 7;
int val;
pipert::AdaptiveSpinLock* g_lock = nullptr;

void SharedIncrementor(int times) {
  ASSERT_TRUE(g_lock);
  for (int i = 0; i < times; i++) {
    std::lock_guard<pipert::AdaptiveSpinLock> guard(*g_lock);
    val++;
  }
}

TEST(AdaptiveSpinLockTest, ThreadsSyncedCorrectly) {
  const int cycles = 100;
  std::thread threads[thread_num];
  val = 0;
  g_lock = new pipert::AdaptiveSpinLock();
  for (int i = 0; i < thread_num; i++)
    threads[i] = std::thread(SharedIncrementor, cycles);
  for (int i = 0; i < thread_num; i++) threads[i].join();
  EXPECT_EQ(val, thread_num * cycles);
  delete g_lock;
  g_lock = nullptr;
}