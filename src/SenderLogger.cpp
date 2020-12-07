#include "SenderLogger.h"

#include <algorithm>
#include <cassert>
#include <cstring>
#include <mutex>

#include "ThreadId.h"

namespace pipert {

SenderLogger::SenderLogger(const char* track_event)
    : track_event_(track_event) {
  ClearTopSenders();
}

void SenderLogger::TrackSender(const char* event_name) {
  if (event_name != track_event_) {
    return;
  }
  ThreadId this_thread = ThreadId::GetCurrentThread();
  const char* thread_name = this_thread.GetName();
  {
    std::lock_guard<AdaptiveSpinLock> lock(mutex_);
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
}

std::uint8_t* SenderLogger::SerializeNClear(std::uint8_t* buffer) {
  std::lock_guard<AdaptiveSpinLock> lock(mutex_);
  std::uint8_t* ret = Serialize(buffer);
  ClearTopSenders();
  return ret;
}

std::uint8_t* SenderLogger::Serialize(std::uint8_t* buffer) {
  static_assert(sizeof(std::uint8_t) == sizeof(char), "Weird byte size");
  memcpy(buffer, "SEND", 4);
  buffer += 4;
  const char* top = GetTopSender();
  int len = (int)strlen((char*)top);
  if (len > kMaxSenderNameLength) len = kMaxSenderNameLength;
  memcpy(buffer, top, len);
  buffer += len + 1;
  buffer[-1] = 0;
  return buffer;
}

void SenderLogger::ClearTopSenders() {
  for (auto& e : senders_) {
    e.sender_name_ = nullptr;
    e.sent_packets_ = 0;
  }
}

const char* SenderLogger::GetTopSender() {
  std::partial_sort(senders_.begin(), senders_.begin() + 1, senders_.end());
  return senders_[0].sender_name_;
}

}  // namespace pipert
