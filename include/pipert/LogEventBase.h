#ifndef _LOG_EVENT_BASE_H_
#define _LOG_EVENT_BASE_H_

namespace pipert {

/// Base class of LogEvent. You need to use LogEvent directly instead of this.
class LogEventBase {
 public:
  /// Get the name of the event type this log event represents.
  /// \return The pointer which serves as an ID and which points to the name.
  const char* GetName() const { return event_name_; }

  /// Get the measurement value stored in this log event.
  /// \return The measurement, always stored in a double precision float.
  double GetValue() const { return event_value_; }

 protected:
  LogEventBase(const char* event_name, double event_value)
      : event_name_(event_name), event_value_(event_value) {}

 private:
  const char* event_name_;
  double event_value_;
};

}  // namespace pipert

#endif  // _LOG_EVENT_BASE_H_
