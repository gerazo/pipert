#include "ProfileData.h"

#include <algorithm>
#include <cassert>

#include "ThreadId.h"

namespace pipert {

const char* const ProfileData::kEventPushed = "Packet Pushed";
const char* const ProfileData::kEventRetrieved = "Packet Retrieved";
const char* const ProfileData::kEventExecuteTime = "Execution Time";
const char* const ProfileData::kEventFillTime = "Fill Time";
const char* const ProfileData::kEventReadTime = "Read Time";

ProfileData::ProfileData(const char* data_group_name)
    : data_group_name_(data_group_name) {
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
  TrackSender(event_name);
}

const char* ProfileData::GetName() const { return data_group_name_; }

const char* ProfileData::GetTopSender() {
  // std::sort(senders_.begin(), senders_.end());
  std::partial_sort(senders_.begin(), senders_.begin() + 1, senders_.end());
  return senders_[0].sender_name_;
}

void ProfileData::ClearTopSenders() {
  for (auto& e : senders_) {
    e.sender_name_ = nullptr;
    e.sent_packets_ = 0;
  }
}

void ProfileData::TrackSender(const char* event_name) {
  if (event_name != kEventPushed) {
    return;
  }
  ThreadId this_thread = ThreadId::GetCurrentThread();
  const char* thread_name = this_thread.GetName();
  int i = 0;
  while (i < (int)senders_.size() && senders_[i].sender_name_ != nullptr &&
         senders_[i].sender_name_ != thread_name)
    i++;
  if (i < (int)senders_.size()) {
    if (senders_[i].sender_name_ == nullptr) {
      senders_[i].sender_name_ = thread_name;
      senders_[i].sent_packets_ = 1;
    } else {
      assert(senders_[i].sender_name_ == thread_name);
      senders_[i].sent_packets_++;
    }
  }
}

}  // namespace pipert
