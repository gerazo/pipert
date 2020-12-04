#include "ThreadId.h"

#include <cassert>

namespace pipert {

thread_local int ThreadId::current_thread_id_ = ThreadId::kUntaggedValue;

thread_local const char* ThreadId::current_thread_name_ = "N/A";

std::atomic<int> ThreadId::hand_tag_next_(ThreadId::kUntaggedValue + 1);

std::atomic<int> ThreadId::auto_tag_next_(ThreadId::kUntaggedValue - 1);

ThreadId ThreadId::GetCurrentThread() {
  if (IsCurrentThreadUntagged()) {
    current_thread_id_ = auto_tag_next_.fetch_sub(1, std::memory_order_acq_rel);
  }
  return ThreadId(current_thread_id_, current_thread_name_);
}

void ThreadId::TagCurrentThread() {
  if (IsCurrentThreadUntagged()) {
    current_thread_id_ = hand_tag_next_.fetch_add(1, std::memory_order_acq_rel);
  } else {
    assert(false);
  }
}

void ThreadId::SetNameOfCurrentThread(const char* thread_name) {
  assert(current_thread_id_ > kUntaggedValue);
  current_thread_name_ = thread_name;
}

bool ThreadId::IsCurrentThreadUntagged() {
  return current_thread_id_ == kUntaggedValue;
}

bool ThreadId::IsHandTagged() const { return thread_id_ > kUntaggedValue; }

bool ThreadId::IsAutoTagged() const { return thread_id_ < kUntaggedValue; }

int_least32_t ThreadId::GetIdForSerialization() const {
  return (int_least32_t)thread_id_;
}

const char* ThreadId::GetName() const { return thread_name_; }

ThreadId::ThreadId(int thread_id, const char* thread_name)
    : thread_id_(thread_id), thread_name_(thread_name) {
  assert(thread_id != kUntaggedValue);
}

}  // namespace pipert