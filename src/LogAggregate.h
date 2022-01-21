#ifndef _LOG_AGGREGATE_H_
#define _LOG_AGGREGATE_H_

#include <cstdint>

#include "pipert/Timer.h"

#include "AdaptiveSpinLock.h"

namespace pipert {

/// (_Part of internal implementation_.)
/// Aggregates log events instead of logging them individually.
///
/// This class is the final target of log events.
/// It measures the time spent since the last time data was cleared
/// and gathers other statistics while summing events.
/// It can serialize its data for a log collector.
class LogAggregate {
 public:
  /// Constructs an empty event aggregator.
  LogAggregate();
  LogAggregate(const LogAggregate&) = delete;
  LogAggregate& operator=(const LogAggregate&) = delete;

  /// Logs an event. This should be used in a multi-threaded scenario.
  /// \param event_value The connected value to the event if a measurement
  ///                    took place.
  /// \note This is a thread-safe function having an object-local granularity.
  void Log(double event_value);

  /// Gathers data in the aggregate, serializes it and clears the aggregate.
  /// This should be used in a multi-threaded scenario.
  /// \param buffer Pointer to serialization buffer where data should be put.
  /// \return Pointer to the place in the buffer after the serialized data.
  /// \note This is a thread-safe function having an object-local granularity.
  std::uint8_t* SerializeNClear(std::uint8_t* buffer);

  /// Clears data in the aggregate.
  /// \note This function is NOT thread-safe, better to be used internally.
  void Clear();

  /// Serializes data into buffer.
  ///
  /// The serialized data does not have a header as the event name is not
  /// stored here.
  /// \see SerializeNClear().
  /// \note This function is NOT thread-safe, better to be used internally.
  std::uint8_t* Serialize(std::uint8_t* buffer) const;

  /// Get how many times the event occured.
  /// \note This function is NOT thread-safe, better to be used internally.
  int GetLogCount() const;

  /// Get the time passed since the start of aggregation.
  /// \return Time passed in microseconds.
  /// \note This function is NOT thread-safe, better to be used internally.
  int GetTimePassedInMicroSecs() const;

  /// Get minimal value logged.
  /// \note This function is NOT thread-safe, better to be used internally.
  double GetMin() const;

  /// Get maximal value logged.
  /// \note This function is NOT thread-safe, better to be used internally.
  double GetMax() const;

  /// Get average of logged values.
  /// \note This function is NOT thread-safe, better to be used internally.
  double GetAvg() const;

 private:
  AdaptiveSpinLock mutex_;
  int log_count_;
  Timer::Time aggregation_started_;
  double min_;
  double max_;
  double sum_;
};

}  // namespace pipert

#endif  //_LOG_AGGREGATE_H_
