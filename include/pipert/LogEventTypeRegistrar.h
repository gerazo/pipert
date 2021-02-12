#ifndef _LOG_EVENT_TYPE_REGISTRAR_H_
#define _LOG_EVENT_TYPE_REGISTRAR_H_

namespace pipert {

template <const char* TYPE_ID>
class LogEvent;

/// This class is responsible to enlist all available event types which can be
/// instantiated in the system.
///
/// This information is automatically collected from all instantiations of
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

}  // namespace pipert

#endif  // _LOG_EVENT_TYPE_REGISTRAR_H_
