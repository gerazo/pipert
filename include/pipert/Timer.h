#ifndef _TIMER_H_
#define _TIMER_H_

#include <chrono>
#include <cstdint>

namespace pipert {

/// General timing utility for monitoring and profiling.
///
/// Its features are:
/// - Defines the format in which all timestamps in the library are kept.
/// - Gives direct access to `std::chrono::high_resolution_clock`.
/// - Allows short term time measurements for internal usage.
class Timer {
 public:
  /// Time format of all timestamps and measurements used in the library
  /// in __microseconds__ units.
  using Time = int_least64_t;

  /// Timeline start.
  static const Time kMinTime = INT_LEAST64_MIN;

  /// Timeline end.
  static const Time kMaxTime = INT_LEAST64_MAX;

  /// \return Time spent since _epoch_ in __microseconds__.
  static Time time();

  /// Start measuring time interval.
  void tic();

  /// End time interval measurement.
  /// \return Time passed in __milliseconds__ in floating point format.
  double toc() const;

  /// End time interval measurement.
  /// \return Time passed in __microseconds__ in integer format.
  int64_t toc_in_us() const;

 private:
  using Clock = std::chrono::high_resolution_clock;
  std::chrono::time_point<Clock> beg_;
};

}  // namespace pipert

#endif  //_TIMER_H_