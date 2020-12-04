#ifndef _LOGAGGREGATE_H_
#define _LOGAGGREGATE_H_

#include <cstdint>

#include "pipert/Timer.h"

namespace pipert {

class LogAggregate {
 public:
  LogAggregate();

  void Clear();
  void Log(double event_value);
  std::uint8_t* Serialize(std::uint8_t* buffer) const;

  int GetLogCount() const;
  int GetTimePassedInMicroSecs() const;
  double GetMin() const;
  double GetMax() const;
  double GetAvg() const;

 private:
  int log_count_;
  Timer::Time aggregation_started_;
  double min_;
  double max_;
  double sum_;
};

}  // namespace pipert

#endif  //_LOGAGGREGATE_H_
