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

namespace {

class SharedIncrementStore {
 public:
  SharedIncrementStore(const int threads) : threads_(threads){};

 public:
  void Increment(int cycles) {
    std::lock_guard<pipert::AdaptiveSpinLock> guard(slock_);
    for (int i = 0; i < cycles; ++i) val_++;
  }
  int GetVal() const { return val_; }

 private:
  pipert::AdaptiveSpinLock slock_;
  const int threads_;
  int val_;
};

static SharedIncrementStore& SharedIncrementorInstance(const int cycles) {
  static SharedIncrementStore si(cycles);
  return si;
}

}  // namespace

TEST(AdaptiveSpinLockTest, ThreadsSyncCorrectly) {
  const int cycles = 50000;
  const int thread_num = 20;
  std::thread threads[thread_num];
  SharedIncrementStore& shinc = SharedIncrementorInstance(cycles);
  for (int i = 0; i < thread_num; i++)
    threads[i] = std::thread(&SharedIncrementStore::Increment, &shinc, cycles);
  for (int i = 0; i < thread_num; i++) threads[i].join();
  EXPECT_EQ(shinc.GetVal(), thread_num * cycles);
}