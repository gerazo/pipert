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
  void SetCycles(const int cycles) { cycles_ = cycles; }
  int GetCycles() const { return cycles_; }
  void ResetVal() { val_ = 0; }
  void IncrementVal() { val_++; }
  int GetVal() const { return val_; }

 private:
  std::vector<std::thread> threads_;
  int cycles_;
  int val_;
};

static SharedIncrementor& SharedIncrementorInstance() {
  static SharedIncrementor si;
  return si;
}

void LockEveryStep(SharedIncrementor& shinc) {
  static pipert::AdaptiveSpinLock slock(2);
  for(int i = 0; i < shinc.GetCycles(); ++i) {
    std::lock_guard<pipert::AdaptiveSpinLock> guard(slock);
    shinc.IncrementVal();
  }
}
void LockYieldEveryStep(SharedIncrementor& shinc) {
  static pipert::AdaptiveSpinLock slock(2);
  for(int i = 0; i < shinc.GetCycles(); ++i) {
    {
      std::lock_guard<pipert::AdaptiveSpinLock> guard(slock);
      shinc.IncrementVal();
    }
    std::this_thread::yield();
  }
}
void LockOnce(SharedIncrementor& shinc) {
  static pipert::AdaptiveSpinLock slock(1);
  std::lock_guard<pipert::AdaptiveSpinLock> guard(slock);
  for(int i = 0; i < shinc.GetCycles(); ++i) {
    shinc.IncrementVal();
  }
}

}  // namespace

TEST(AdaptiveSpinLockTest, ThreadsLockEveryStep) {
  SharedIncrementor& shinc = SharedIncrementorInstance();

  shinc.SetThreads(std::thread::hardware_concurrency() * 2);
  shinc.SetCycles(1000);
  shinc.ResetVal();
  shinc.StartThreads(LockEveryStep);
  shinc.JoinThreads();
  EXPECT_EQ(shinc.GetVal(), shinc.GetThreads() * shinc.GetCycles());
}

TEST(AdaptiveSpinLockTest, ThreadsLockAndYieldEveryStep) {
  SharedIncrementor& shinc = SharedIncrementorInstance();

  shinc.SetThreads(std::thread::hardware_concurrency() * 2);
  shinc.SetCycles(1000);
  shinc.ResetVal();
  shinc.StartThreads(LockYieldEveryStep);
  shinc.JoinThreads();
  EXPECT_EQ(shinc.GetVal(), shinc.GetThreads() * shinc.GetCycles());
}

TEST(AdaptiveSpinLockTest, ThreadsLockOnce) {
  SharedIncrementor& shinc = SharedIncrementorInstance();

  shinc.SetThreads(std::thread::hardware_concurrency() * 2);
  shinc.SetCycles(10000);
  shinc.ResetVal();
  shinc.StartThreads(LockOnce);
  shinc.JoinThreads();
  EXPECT_EQ(shinc.GetVal(), shinc.GetThreads() * shinc.GetCycles());
}