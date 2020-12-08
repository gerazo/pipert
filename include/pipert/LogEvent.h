#ifndef _LOG_EVENT_H_
#define _LOG_EVENT_H_

namespace pipert {

class LogEventTypeRegistrar;

/// Encapsulates a logged event type and one corresponding measurement.
///
/// The instances of this class can be sent to the logging facility, so
/// the events can be logged, aggregated and sent to a log processor later on.
/// \tparam TYPE_ID ID of the event type of this event.
///                 Although this ID points to a string which is indeed used
///                 when logging the event, this pointer is used as a real,
///                 unique ID here. So if you have two identical strings in
///                 memory, they will still mean different event types despite
///                 having the same name. It does make sense to use
///                 `const char str[]` global variables or constants as IDs.
template <const char* TYPE_ID>
class LogEvent {
 public:
  /// Constructs a LogEvent object.
  /// \param event_value The measurement the log processor needs to
  ///                    make statistics from.
  LogEvent(double event_value) : value_(event_value) { (void)registrar_; }

  /// Get the name of the event type this log event represents.
  /// \return The pointer which serves as an ID and which points to the name.
  const char* GetName() const { return TYPE_ID; }

  /// Get the measurement value stored in this log event.
  /// \return The measurement, value always stored in a double precision float.
  double GetValue() const { return value_; }

 private:
  static LogEventTypeRegistrar registrar_;

  double value_;
};

/// This class is responsible to enlist all available event types which can be
/// instantiated in the system.
///
/// This information is automagically collected from all instantiations of
/// LogEvent in the whole link unit.
/// Use LogEvent directly and not this class!
class LogEventTypeRegistrar {
 public:
  /// Get the length of the array which stores all the available
  /// log event types which can be instantiated in the system.
  static int GetRegisterLength();

  /// Gets a log event type from the event type list. See GetRegisterLength().
  static const char* GetLogEventTypeID(int i);

 private:
  static const int kLogEventTypeRegisterMaxLength = 256;

  static int register_length_;
  static const char* register_ids_[kLogEventTypeRegisterMaxLength];

  template <const char* TYPE_ID>
  friend class LogEvent;
  LogEventTypeRegistrar(const char* type_id);
};

template <const char* TYPE_ID>
LogEventTypeRegistrar LogEvent<TYPE_ID>::registrar_(TYPE_ID);

}  // namespace pipert

#endif  // _LOG_EVENT_H_
