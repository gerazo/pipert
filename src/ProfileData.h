#ifndef _PROFILEDATA_H_
#define _PROFILEDATA_H_

#include "LogAggregate.h"
#include "SenderLogger.h"

#include <unordered_map>

namespace pipert {

class ProfileData {
 public:
  static const char* const kEventPushed;
  static const char* const kEventRetrieved;
  static const char* const kEventExecuteTime;
  static const char* const kEventFillTime;
  static const char* const kEventReadTime;

  ProfileData(const char* data_group_name);

  void InitEvent(const char* event_name);
  void Log(const char* event_name, double event_value);

  const char* GetName() const;

 private:
  using LogAggregateKey = const char*;

  const char* data_group_name_;
  std::unordered_map<LogAggregateKey, LogAggregate> aggregates_;
  SenderLogger sender_logger_;
};

}  // namespace pipert

#endif  // _PROFILEDATA_H_
