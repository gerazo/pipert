#ifndef _PROFILE_DATA_H_
#define _PROFILE_DATA_H_

#include "pipert/LogEventBase.h"

#include "LogAggregate.h"
#include "SenderLogger.h"

#include <unordered_map>

namespace pipert {

class ProfileData {
 public:
  static const char kEventPushed[];
  static const char kEventRetrieved[];
  static const char kEventExecuteTime[];
  static const char kEventFillTime[];
  static const char kEventReadTime[];

  ProfileData(const char* data_group_name);

  void Log(LogEventBase log_event);

  const char* GetName() const;

 private:
  using LogAggregateKey = const char*;

  const char* data_group_name_;
  std::unordered_map<LogAggregateKey, LogAggregate> aggregates_;
  SenderLogger sender_logger_;
};

}  // namespace pipert

#endif  // _PROFILE_DATA_H_
