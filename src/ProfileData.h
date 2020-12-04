#ifndef _PROFILEDATA_H_
#define _PROFILEDATA_H_

#include "LogAggregate.h"

#include <array>
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

  const char* GetTopSender();
  void ClearTopSenders();

 private:
  using LogAggregateKey = const char*;

  struct Sender {
    Sender() : sender_name_(nullptr), sent_packets_(0) {}
    bool operator<(const Sender& o) const {
      return sent_packets_ > o.sent_packets_;
    }

    const char* sender_name_;
    int sent_packets_;
  };

  static const int kMaxSenders = 4;

  void TrackSender(const char* event_name);

  const char* data_group_name_;
  std::array<Sender, kMaxSenders> senders_;
  std::unordered_map<LogAggregateKey, LogAggregate> aggregates_;
};

}  // namespace pipert

#endif  // _PROFILEDATA_H_
