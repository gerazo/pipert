#include "LogAggregate.h"

#include <cassert>
#include <limits>

#include "Endianness.h"

namespace pipert {

LogAggregate::LogAggregate() { Clear(); }

void LogAggregate::Clear() {
  log_count_ = 0;
  aggregation_started_ = Timer::time();
  min_ = std::numeric_limits<double>::max();
  max_ = std::numeric_limits<double>::min();
  sum_ = 0.0;
}

void LogAggregate::Log(double event_value) {
  log_count_++;
  sum_ += event_value;
  if (event_value < min_) min_ = event_value;
  if (event_value > max_) max_ = event_value;
}

std::uint8_t* LogAggregate::Serialize(std::uint8_t* buffer) const {
  *(std::int32_t*)buffer =
      ConvertHostToNetworkByteOrder((std::int32_t)GetLogCount());
  buffer += 4;
  *(std::int32_t*)buffer =
      ConvertHostToNetworkByteOrder((std::int32_t)GetTimePassedInMicroSecs());
  buffer += 4;
  *(double*)buffer = ConvertHostToNetworkByteOrder(GetMin());
  buffer += 8;
  *(double*)buffer = ConvertHostToNetworkByteOrder(GetMax());
  buffer += 8;
  *(double*)buffer = ConvertHostToNetworkByteOrder(GetAvg());
  buffer += 8;
  return buffer;
}

int LogAggregate::GetLogCount() const { return log_count_; }

int LogAggregate::GetTimePassedInMicroSecs() const {
  Timer::Time t = Timer::time();
  assert(t - aggregation_started_ < std::numeric_limits<int>::max());
  return (int)(t - aggregation_started_);
}

double LogAggregate::GetMin() const { return min_; }

double LogAggregate::GetMax() const { return max_; }

double LogAggregate::GetAvg() const {
  return log_count_ ? sum_ / log_count_ : 0.0;
}

}  // namespace pipert
