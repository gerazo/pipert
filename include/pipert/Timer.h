#ifndef _TIMER_H_
#define _TIMER_H_

#include <chrono>
#include <cstdint>

namespace pipert {

class Timer {
 public:
  using Time = uint64_t;

  static int64_t time();  // return time since epoch in microseconds

  void tic();
  double toc() const;         // return time passed in milliseconds
  int64_t toc_in_us() const;  // return time passed in microseconds

 private:
  using Clock = std::chrono::high_resolution_clock;
  std::chrono::time_point<Clock> beg_;
};

}  // namespace pipert

#endif  //_TIMER_H_