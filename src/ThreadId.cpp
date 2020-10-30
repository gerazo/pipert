#include "ThreadId.h"

#include <cassert>

namespace pipert {

thread_local int ThreadId::current_thread_id_ = ThreadId::kUntaggedValue;

std::atomic<int> ThreadId::hand_tag_next_(ThreadId::kUntaggedValue + 1);

std::atomic<int> ThreadId::auto_tag_next_(ThreadId::kUntaggedValue - 1);

ThreadId ThreadId::GetCurrentThread() {
  if (IsCurrentThreadUntagged()) {
    current_thread_id_ = auto_tag_next_.fetch_sub(1, std::memory_order_acq_rel);
  }
  return ThreadId(current_thread_id_);
}

void ThreadId::TagCurrentThread() {
  if (IsCurrentThreadUntagged()) {
    current_thread_id_ = hand_tag_next_.fetch_add(1, std::memory_order_acq_rel);
  } else {
    assert(false);
  }
}

bool ThreadId::IsCurrentThreadUntagged() {
  return current_thread_id_ == kUntaggedValue;
}

bool ThreadId::IsHandTagged() const { return thread_id_ > kUntaggedValue; }

bool ThreadId::IsAutoTagged() const { return thread_id_ < kUntaggedValue; }

int_least32_t ThreadId::GetIdForSerialization() {
  return (int_least32_t)thread_id_;
}

ThreadId::ThreadId(int thread_id) : thread_id_(thread_id) {
  assert(thread_id != kUntaggedValue);
}

}  // namespace pipert