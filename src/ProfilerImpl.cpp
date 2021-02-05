#include "ProfilerImpl.h"

#include <algorithm>
#include <cassert>

#include "ProfileData.h"

namespace pipert {

ProfilerImpl::ProfilerImpl(SenderCallback sender_callback, int buffer_size,
                           int aggregation_time_msec)
    : sender_callback_(sender_callback),
      buffer_size_(buffer_size),
      aggregation_time_msec_(aggregation_time_msec),
      worker_shutdown_(false),
      next_profile_data_(-1) {
  assert(sender_callback_ != nullptr);
  assert(buffer_size_ >= 64);
  assert(aggregation_time_msec_ >= 0);
  buffer_ = new std::uint8_t[buffer_size_];
}

ProfilerImpl::~ProfilerImpl() {
  assert(profile_data_.size() == 0);
  if (aggregation_time_msec_ != 0 && worker_thread_.joinable())
    Stop();
  delete[] buffer_;
}

void ProfilerImpl::AddProfileData(ProfileData* profile_data) {
  assert(profile_data != nullptr);
  assert(!worker_thread_.joinable());
  int expected = -1;
  bool changed = next_profile_data_.compare_exchange_strong(expected, -2);
  if (!changed) {
    assert(false);
    return;
  }
  assert(profile_data_.end() ==
         std::find(profile_data_.begin(), profile_data_.end(), profile_data));
  profile_data_.push_back(profile_data);
  next_profile_data_ = -1;
}

void ProfilerImpl::RemoveProfileData(ProfileData* profile_data) {
  assert(profile_data != nullptr);
  assert(!worker_thread_.joinable());
  int expected = -1;
  bool changed = next_profile_data_.compare_exchange_strong(expected, -2);
  if (!changed) {
    assert(false);
    return;
  }
  auto iter =
      std::find(profile_data_.begin(), profile_data_.end(), profile_data);
  if (iter != profile_data_.end()) {
    *iter = profile_data_.back();
    profile_data_.pop_back();
  } else {
    assert(false);
  }
  next_profile_data_ = -1;
}

void ProfilerImpl::Start() {
  assert(!worker_thread_.joinable());
  worker_shutdown_.store(false, std::memory_order_release);
  if (aggregation_time_msec_ != 0) {
    worker_thread_ = std::thread(&ProfilerImpl::SleepNWork, this);
  }
}

void ProfilerImpl::Stop() {
  worker_shutdown_.store(true, std::memory_order_release);
  if (aggregation_time_msec_ != 0) {
    assert(worker_thread_.joinable());
    worker_thread_.join();
  }
  assert(!worker_thread_.joinable());
}

void ProfilerImpl::GatherNSend() {
  int expected = -1;
  bool changed = next_profile_data_.compare_exchange_strong(expected, 0);
  if (!changed) {
    assert(false);
    return;
  }
  while (next_profile_data_ < (int)profile_data_.size()) {
    ProfileData* data = profile_data_[next_profile_data_];
    bool more;
    do {
      std::uint8_t* buffer_end = buffer_ + buffer_size_;
      more = data->GatherNSerialize(buffer_, buffer_end);
      sender_callback_(buffer_, buffer_end - buffer_);
    } while (more);
    next_profile_data_++;
  }
  next_profile_data_ = -1;
}

void ProfilerImpl::SleepNWork() {
  assert(aggregation_time_msec_ > 0);
  while (!worker_shutdown_.load(std::memory_order_acquire)) {
    GatherNSend();
    std::this_thread::sleep_for(
        std::chrono::milliseconds(aggregation_time_msec_));
  }
}

}  // namespace pipert
