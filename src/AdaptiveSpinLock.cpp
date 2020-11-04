#include "AdaptiveSpinLock.h"

#include <thread>

namespace pipert {

int AdaptiveSpinLock::spins_before_yield_ = 0;

AdaptiveSpinLock::AdaptiveSpinLock(const int spins_before_yield) {
  if (!spins_before_yield_) {
    spins_before_yield_ = ((std::thread::hardware_concurrency() == 1) ? 1 : spins_before_yield);
  }
}

void AdaptiveSpinLock::lock() {
  int countdown = spins_before_yield_;
  while (mutex_.test_and_set(std::memory_order_acquire)) {
    if (!--countdown) {
      countdown = spins_before_yield_;
      std::this_thread::yield();
    }
  }
}

bool AdaptiveSpinLock::try_lock() {
  return !mutex_.test_and_set(std::memory_order_acquire);
}

void AdaptiveSpinLock::unlock() {
  mutex_.clear(std::memory_order_release);
}

}  // namespace pipert