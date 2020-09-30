#ifndef _TIMER_H_
#define _TIMER_H_

#include <chrono>
#include <cstdint>

namespace pipert {

class Timer {
 public:
  using Time = int_least64_t;

  static const Time kMinTime = INT_LEAST64_MIN;
  static const Time kMaxTime = INT_LEAST64_MAX;

  static Time time();  // return time since epoch in microseconds

  void tic();
  double toc() const;         // return time passed in milliseconds
  int64_t toc_in_us() const;  // return time passed in microseconds

 private:
  using Clock = std::chrono::high_resolution_clock;
  std::chrono::time_point<Clock> beg_;
};

}  // namespace pipert

#endif  //_TIMER_H_