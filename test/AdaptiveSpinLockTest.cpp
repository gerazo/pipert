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

class SharedIncrementor {
 public:
  SharedIncrementor() {}
  SharedIncrementor(int spin_cycle_count)
    : slock_(spin_cycle_count) {}

  void StartThreads(void func (SharedIncrementor&)) {
    for(unsigned int i = 0; i < threads_.size(); ++i)
      threads_[i] = std::thread(func, std::ref(*this));
  }
  void JoinThreads() {
    for(unsigned int i = 0; i < threads_.size(); ++i)
      threads_[i].join();
  }

  void SetThreads(const int count) { threads_.resize(count); }
  int GetThreads() const { return threads_.size(); }
  pipert::AdaptiveSpinLock& GetSpinLock() { return slock_; }
  void SetCycles(const int cycles) { cycles_ = cycles; }
  int GetCycles() const { return cycles_; }
  void ResetVal() { val_ = 0; }
  void IncrementVal() { val_++; }
  int GetVal() const { return val_; }

 private:
  std::vector<std::thread> threads_;
  pipert::AdaptiveSpinLock slock_;
  int cycles_;
  int val_;
};

void LockEveryStep(SharedIncrementor& shinc) {
  for(int i = 0; i < shinc.GetCycles(); ++i) {
    std::lock_guard<pipert::AdaptiveSpinLock> guard(shinc.GetSpinLock());
    shinc.IncrementVal();
  }
}
void LockYieldEveryStep(SharedIncrementor& shinc) {
  for(int i = 0; i < shinc.GetCycles(); ++i) {
    {
      std::lock_guard<pipert::AdaptiveSpinLock> guard(shinc.GetSpinLock());
      shinc.IncrementVal();
    }
    std::this_thread::yield();
  }
}
void LockOnce(SharedIncrementor& shinc) {
  std::lock_guard<pipert::AdaptiveSpinLock> guard(shinc.GetSpinLock());
  for(int i = 0; i < shinc.GetCycles(); ++i) {
    shinc.IncrementVal();
  }
}

}  // namespace

TEST(AdaptiveSpinLockTest, ThreadsLockEveryStepWithDefaultSpin) {
  SharedIncrementor shinc; // default ctor to test default spin cycle count

  shinc.SetThreads(std::thread::hardware_concurrency() * 2);
  shinc.SetCycles(1000);
  shinc.ResetVal();
  shinc.StartThreads(LockEveryStep);
  shinc.JoinThreads();
  EXPECT_EQ(shinc.GetVal(), shinc.GetThreads() * shinc.GetCycles());
}

TEST(AdaptiveSpinLockTest, ThreadsLockEveryStepWithShortSpin) {
  SharedIncrementor shinc(2); // few spin cycles, but still multiple cycles

  shinc.SetThreads(std::thread::hardware_concurrency() * 2);
  shinc.SetCycles(1000);
  shinc.ResetVal();
  shinc.StartThreads(LockEveryStep);
  shinc.JoinThreads();
  EXPECT_EQ(shinc.GetVal(), shinc.GetThreads() * shinc.GetCycles());
}

TEST(AdaptiveSpinLockTest, ThreadsLockAndYieldEveryStepWithShortSpin) {
  SharedIncrementor shinc(2); // few spin cycles, but still multiple cycles

  shinc.SetThreads(std::thread::hardware_concurrency() * 2);
  shinc.SetCycles(1000);
  shinc.ResetVal();
  shinc.StartThreads(LockYieldEveryStep);
  shinc.JoinThreads();
  EXPECT_EQ(shinc.GetVal(), shinc.GetThreads() * shinc.GetCycles());
}

TEST(AdaptiveSpinLockTest, ThreadsLockOnceWithSingleSpin) {
  SharedIncrementor shinc(1); // single spin cycle, to force expiration

  shinc.SetThreads(std::thread::hardware_concurrency() * 2);
  shinc.SetCycles(10000);
  shinc.ResetVal();
  shinc.StartThreads(LockOnce);
  shinc.JoinThreads();
  EXPECT_EQ(shinc.GetVal(), shinc.GetThreads() * shinc.GetCycles());
}