#include "ProfileData.h"

#include <cassert>

#include "pipert/LogEventTypeRegistrar.h"

namespace pipert {

const char ProfileData::kEventPushed[] = "Packet Pushed";
const char ProfileData::kEventRetrieved[] = "Packet Retrieved";
const char ProfileData::kEventExecuteTime[] = "Execution Time";
const char ProfileData::kEventFillTime[] = "Fill Time";
const char ProfileData::kEventReadTime[] = "Read Time";

ProfileData::ProfileData(const char* data_group_name)
    : data_group_name_(data_group_name), sender_logger_(kEventPushed) {
  // We should have at least the built-in types registered.
  assert(LogEventTypeRegistrar::GetRegisterLength() >= 5);
  for (int i = 0; i < LogEventTypeRegistrar::GetRegisterLength(); i++) {
    aggregates_[LogEventTypeRegistrar::GetLogEventTypeID(i)];
  }
}

void ProfileData::Log(LogEventBase log_event) {
  auto it = aggregates_.find(log_event.GetName());
  if (it != aggregates_.end()) {
    it->second.Log(log_event.GetValue());
  } else {
    assert(false);  // We cannot have an unregistered event type.
  }
  sender_logger_.TrackSender(log_event.GetName());
}

const char* ProfileData::GetName() const { return data_group_name_; }

}  // namespace pipert
