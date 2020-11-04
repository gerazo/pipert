#ifndef _ADAPTIVESPINLOCK_H_
#define _ADAPTIVESPINLOCK_H_

#include <atomic>

namespace pipert {

/// (_Part of internal implementation._)
/// A mutex implementing a real-time spinlock behavior that tries to not lose
/// its timeslice by avoiding OS calls.
///
/// AdaptiveSpinLock implements _BasicLockable_ and _Lockable_ requirements of
/// _C++11_, and as such, it is perfectly usable with
/// `std::lock_guard`, `std::unique_lock` and similar mutex wrappers.
///
/// It uses `std::atomic_flag` to guarantee HW support for synchronization.
/// On request, it actively waits for the lock to be released by other threads,
/// but after a certain cycle count, it gives back the rest of the timeslice to
/// the OS in order to avoid draining battery or becoming a CPU hog.
/// Use this for short operations to minimize delay in other threads.
class AdaptiveSpinLock {
 public:
  /// Cycles to spin before giving up the rest of the timeslice.
  static const int kSpinsBeforeYield = 256;
  static int spins_before_yield_;

  AdaptiveSpinLock(const int spins_before_yield = kSpinsBeforeYield);
  AdaptiveSpinLock(const AdaptiveSpinLock&) = delete;
  AdaptiveSpinLock& operator=(const AdaptiveSpinLock&) = delete;

  /// Acquire the lock of this mutex by
  /// actively waiting with minimal delay until success.
  void lock();

  /// Try to acquire mutex lock and return without delay.
  /// \return True if lock was acquired successfully.
  bool try_lock();

  /// Release the lock of this mutex.
  void unlock();

 private:
  std::atomic_flag mutex_ = ATOMIC_FLAG_INIT;
};

}  // namespace pipert

#endif  //_ADAPTIVESPINLOCK_H_