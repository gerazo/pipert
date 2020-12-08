#ifndef _PROFILE_DATA_H_
#define _PROFILE_DATA_H_

#include "pipert/LogEventBase.h"

#include <cstdint>
#include <unordered_map>

#include "LogAggregate.h"
#include "SenderLogger.h"

namespace pipert {

class ProfileData {
 public:
  static const char kEventPushed[];
  static const char kEventRetrieved[];
  static const char kEventExecuteTime[];
  static const char kEventFillTime[];
  static const char kEventReadTime[];

  /// Maximum length of all names (group and event) that will be serialized.
  static const int kMaxNameLength = 27;

  ProfileData(const char* data_group_name);

  const char* GetName() const;

  void Log(LogEventBase log_event);

  bool GatherNSerialize(std::uint8_t* buffer, std::uint8_t*& buffer_end);

 private:
  using LogAggregateKey = const char*;

  static std::uint8_t* SerializeString(std::uint8_t* buf, const char* str);

  const char* data_group_name_;
  std::unordered_map<LogAggregateKey, LogAggregate> aggregates_;
  SenderLogger sender_logger_;

  std::unordered_map<LogAggregateKey, LogAggregate>::iterator serialize_next_;
};

}  // namespace pipert

#endif  // _PROFILE_DATA_H_
