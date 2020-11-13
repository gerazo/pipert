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
  /// Default number of cycles to spin before giving up the rest of the
  /// timeslice.
  static const int kSpinsBeforeYield = 256;

  /// Construct an AdaptiveSpinLock mutex.
  ///
  /// \param spins_before_yield An optional parameter that corresponds to the
  ///        amount of cycles the lock will spin for while trying to lock its
  ///        mutex. Because of differences between specific hardware and the
  ///        behavior of OS schedulers, the optimal setting for this parameter
  ///        changes between use cases. It is only useful to experiment with
  ///        this parameter if you really know what you are doing, and even
  ///        then, it will probably require quite some experimentation to find
  ///        the setting most suitable for your particular use case.
  ///        Higher values will result in more busy waiting for locks and
  ///        therefore more wasted CPU time, while lower values will shorten
  ///        the time before a the lock gives up its timeslice, possibly
  ///        leading to higher delay on lock acquisitions.
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
  int spins_before_yield_ = 0;
};

}  // namespace pipert

#endif  //_ADAPTIVESPINLOCK_H_