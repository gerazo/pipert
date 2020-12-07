#include "ProfileData.h"

#include <cassert>

namespace pipert {

const char* const ProfileData::kEventPushed = "Packet Pushed";
const char* const ProfileData::kEventRetrieved = "Packet Retrieved";
const char* const ProfileData::kEventExecuteTime = "Execution Time";
const char* const ProfileData::kEventFillTime = "Fill Time";
const char* const ProfileData::kEventReadTime = "Read Time";

ProfileData::ProfileData(const char* data_group_name)
    : data_group_name_(data_group_name), sender_logger_(kEventPushed) {
  InitEvent(kEventPushed);
  InitEvent(kEventRetrieved);
  InitEvent(kEventExecuteTime);
  InitEvent(kEventFillTime);
  InitEvent(kEventReadTime);
}

void ProfileData::InitEvent(const char* event_name) {
  aggregates_[event_name];
}

void ProfileData::Log(const char* event_name, double event_value) {
  auto it = aggregates_.find(event_name);
  if (it != aggregates_.end()) {
    it->second.Log(event_value);
  } else {
    assert(false);
  }
  sender_logger_.TrackSender(event_name);
}

const char* ProfileData::GetName() const { return data_group_name_; }

}  // namespace pipert
