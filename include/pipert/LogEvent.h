#ifndef _LOG_EVENT_H_
#define _LOG_EVENT_H_

#include "pipert/LogEventBase.h"
#include "pipert/LogEventTypeRegistrar.h"

namespace pipert {

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
class LogEvent : public LogEventBase {
 public:
  /// Constructs a LogEvent object.
  /// \param event_value The measurement the log processor needs to
  ///                    make statistics from.
  LogEvent(double event_value) : LogEventBase(TYPE_ID, event_value) {
    (void)registrar_;
  }

 private:
  static LogEventTypeRegistrar registrar_;
};

template <const char* TYPE_ID>
LogEventTypeRegistrar LogEvent<TYPE_ID>::registrar_(TYPE_ID);

}  // namespace pipert

#endif  // _LOG_EVENT_H_
