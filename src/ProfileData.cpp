#include "ProfileData.h"

#include <cassert>
#include <cstring>

#include "pipert/LogEventTypeRegistrar.h"

namespace pipert {

const char ProfileData::kEventPushed[] = "Packet Pushed";
const char ProfileData::kEventRetrieved[] = "Packet Retrieved";
const char ProfileData::kEventExecuteTime[] = "Execution Time";
const char ProfileData::kEventFillTime[] = "Fill Time";
const char ProfileData::kEventReadTime[] = "Read Time";
const char ProfileData::kEventDroppedPacket[] = "Packet Dropped";

ProfileData::ProfileData(const char* data_group_name)
    : data_group_name_(data_group_name), sender_logger_(kEventPushed) {
  assert(LogEventTypeRegistrar::GetRegisterLength() >= 0);

  // prepare the hash to contain every possible event type
  for (int i = 0; i < LogEventTypeRegistrar::GetRegisterLength(); i++) {
    aggregates_[LogEventTypeRegistrar::GetLogEventTypeID(i)];
  }

  serialize_next_ = aggregates_.end();
}

const char* ProfileData::GetName() const { return data_group_name_; }

void ProfileData::Log(LogEventBase log_event) {
  auto it = aggregates_.find(log_event.GetName());
  if (it != aggregates_.end()) {
    it->second.Log(log_event.GetValue());
  } else {
    assert(false);  // We cannot have an unregistered event type.
  }
  sender_logger_.TrackSender(log_event.GetName());
}

bool ProfileData::GatherNSerialize(std::uint8_t* buffer,
                                   std::uint8_t*& buffer_end) {
  const int safety_limit = (buffer_end - buffer) - 64;
  assert(safety_limit >= 64);
  std::uint8_t* buf = buffer;
  // compulsory header (max 32 bytes)
  memcpy(buf, "DGRP", 4);
  buf += 4;
  buf = SerializeString(buf, data_group_name_);
  assert(buf <= buffer_end);
  if (serialize_next_ == aggregates_.end()) {
    buf = sender_logger_.SerializeNClear(buf);
    serialize_next_ = aggregates_.begin();
  }
  assert(buf <= buffer_end);
  while (buf <= buffer + safety_limit && serialize_next_ != aggregates_.end()) {
    std::uint8_t* orig_buf = buf;
    memcpy(buf, "LOGA", 4);
    buf += 4;
    buf = SerializeString(buf, serialize_next_->first);
    assert(buf <= buffer_end);
    std::int32_t* log_count = (std::int32_t*)buf;
    buf = serialize_next_->second.SerializeNClear(buf);
    assert(buf <= buffer_end);
    serialize_next_++;
    if (*log_count == 0) {
      // we skip empty data, so we remove this from the buffer
      buf = orig_buf;
    }
  }
  buffer_end = buf;
  return serialize_next_ != aggregates_.end();
}

std::uint8_t* ProfileData::SerializeString(std::uint8_t* buf, const char* str) {
  int len = (int)strlen(str);
  if (len > kMaxNameLength) len = kMaxNameLength;
  memcpy(buf, str, len);
  buf += len + 1;
  buf[-1] = 0;
  return buf;
}

}  // namespace pipert
