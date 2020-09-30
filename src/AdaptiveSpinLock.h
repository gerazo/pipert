#ifndef _ADAPTIVESPINLOCK_H_
#define _ADAPTIVESPINLOCK_H_

#include <atomic>

namespace pipert {

/// Spin lock which allows OS to schedule a bit to avoid longer active spins
/// Use this with std::lock_guard
class AdaptiveSpinLock {
 public:
  static const int kSpinsBeforeYield = 256;
  static int spins_before_yield_;

  AdaptiveSpinLock();
  AdaptiveSpinLock(const AdaptiveSpinLock&) = delete;
  AdaptiveSpinLock& operator=(const AdaptiveSpinLock&) = delete;

  void lock();
  bool try_lock();
  void unlock();

 private:
  std::atomic_flag mutex_ = ATOMIC_FLAG_INIT;
};

}  // namespace pipert

#endif  //_ADAPTIVESPINLOCK_H_