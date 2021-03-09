#include "ChannelImpl.h"

#include <algorithm>
#include <cassert>
#include <mutex>

#include "pipert/LogEvent.h"
#include "pipert/Timer.h"

#include "ProfileData.h"
#include "SchedulerImpl.h"

namespace pipert {

ChannelImpl::ChannelImpl(const char* name, int capacity, int packet_size,
                         void* single_thread_object,
                         ChannelBase::InternalCallback callback,
                         SchedulerImpl* scheduler)
    : single_thread_object_(single_thread_object),
      callback_(callback),
      queued_packets_(PacketOrdering()),
      name_(name),
      capacity_(capacity),
      packet_size_(packet_size),
      scheduler_(scheduler),
      base_(nullptr),
      profile_data_(nullptr) {
  pool_ = new int8_t[capacity_ * packet_size_];
  assert(pool_);
  free_packets_.reserve(capacity_);
  for (int i = capacity_ - 1; i >= 0; i--) {
    free_packets_.push_back(
        reinterpret_cast<PacketBase*>(pool_ + packet_size_ * i));
  }
  assert((int)free_packets_.size() == capacity_);
  assert((int)free_packets_.capacity() >= capacity_);
  queued_packets_.reserve(capacity_);
  assert(queued_packets_.size() == 0);
  assert((int)queued_packets_.capacity() >= capacity_);
  scheduler_->RegisterChannel(this);
}

ChannelImpl::~ChannelImpl() {
  scheduler_->UnregisterChannel(this);
  assert(pool_);
  delete[] pool_;
  pool_ = nullptr;
}

PacketBase* ChannelImpl::Acquire() {
  PacketBase* packet = nullptr;
  for (int i = 0; !packet && i < 2 && (i == 0 || TryDroppingPacket()); i++) {
    std::lock_guard<AdaptiveSpinLock> lock(free_mutex_);
    if (!free_packets_.empty()) {
      packet = free_packets_.back();
      free_packets_.pop_back();
    }
  }
  assert(!packet || reinterpret_cast<int8_t*>(packet) >= pool_);
  assert(!packet ||
         reinterpret_cast<int8_t*>(packet) < pool_ + packet_size_ * capacity_);
  assert(!packet ||
         (reinterpret_cast<int8_t*>(packet) - pool_) % packet_size_ == 0);
  if (packet) packet->op_timestamp_ = Timer::time();
  return packet;
}

void ChannelImpl::Push(PacketBase* packet) {
  Timer::Time t = Timer::time();
  assert(t - packet->timestamp_ >= 0);  // bad packet timestamp?
  assert(t - packet->op_timestamp_ >= 0);  // bad operation timestamp?
  Log(LogEvent<ProfileData::kEventPushed>(t - packet->timestamp_));
  Log(LogEvent<ProfileData::kEventFillTime>(t - packet->op_timestamp_));
  std::lock_guard<AdaptiveSpinLock> lock(GetQueuedMutex());
  assert((int)queued_packets_.size() < capacity_);
  Timer::Time earliest = queued_packets_.empty() ? Timer::kMaxTime : PeekNext();
  queued_packets_.push_heap(packet);
  if (IsScheduled() && PeekNext() < earliest) {
    if (earliest == Timer::kMaxTime)
      scheduler_->JobsArrived(this);
    else
      scheduler_->JobsUpdated(this, true);
  }
}

PacketBase* ChannelImpl::GetNext(bool dropping_packet) {
  std::lock_guard<AdaptiveSpinLock> lock(GetQueuedMutex());
  return !queued_packets_.empty() ? PopNext(dropping_packet) : nullptr;
}

void ChannelImpl::Release(PacketBase* packet, bool dropping_packet) {
  if (!dropping_packet) {
    Timer::Time t = Timer::time();
    assert(t - packet->op_timestamp_ >= 0);  // bad operation timestamp?
    Log(LogEvent<ProfileData::kEventReadTime>(t - packet->op_timestamp_));
  }
  assert(reinterpret_cast<int8_t*>(packet) >= pool_);
  assert(reinterpret_cast<int8_t*>(packet) < pool_ + packet_size_ * capacity_);
  assert((reinterpret_cast<int8_t*>(packet) - pool_) % packet_size_ == 0);
  std::lock_guard<AdaptiveSpinLock> lock(free_mutex_);
  assert((int)free_packets_.size() < capacity_);
  free_packets_.push_back(packet);
}

Timer::Time ChannelImpl::PeekNext() const {
  // No locking as it can only be called from Scheduler or here
  assert(!queued_packets_.empty());
  return queued_packets_.front()->timestamp();
}

PacketBase* ChannelImpl::PopNext(bool dropping_packet) {
  // No locking as it is called from Scheduler or here
  assert((int)queued_packets_.size() <= capacity_);
  assert(!queued_packets_.empty());
  PacketBase* earliest = queued_packets_.front();
  queued_packets_.pop_heap();
  if (IsScheduled()) {
    if (queued_packets_.empty())
      scheduler_->JobsDropped(this);
    else if (earliest != queued_packets_.front())
      scheduler_->JobsUpdated(this, false);
  }
  if (!dropping_packet) {
    // TODO do it outside the lock (called from 2 places)
    Timer::Time t = Timer::time();
    earliest->op_timestamp_ = t;
    assert(t - earliest->timestamp_ >= 0);  // bad packet timestamp?
    Log(LogEvent<ProfileData::kEventRetrieved>(t - earliest->timestamp_));
  }
  return earliest;
}

void ChannelImpl::Execute(PacketBase* packet) {
  assert(base_);
  assert(packet);
  Timer::Time t1 = Timer::time();
  callback_(base_, packet);
  Timer::Time t2 = Timer::time();
  Log(LogEvent<ProfileData::kEventExecuteTime>(t2 - t1));
}

int ChannelImpl::GetFreeBufferLength() {
  std::lock_guard<AdaptiveSpinLock> lock(free_mutex_);
  return free_packets_.size();
}

int ChannelImpl::GetQueuedBufferLength() {
  std::lock_guard<AdaptiveSpinLock> lock(GetQueuedMutex());
  return queued_packets_.size();
}

bool ChannelImpl::PacketOrdering::operator()(const PacketBase* a, const PacketBase* b) {
  return a->timestamp() > b->timestamp();
}

void ChannelImpl::SetBase(ChannelBase* base) { base_ = base; }

void ChannelImpl::Log(LogEventBase log_event) {
  if (profile_data_) {
    profile_data_->Log(log_event);
  }
}

void ChannelImpl::SetProfileData(ProfileData* profile_data) {
  profile_data_ = profile_data;
}

ProfileData* ChannelImpl::GetProfileData() {
  return profile_data_;
}

AdaptiveSpinLock& ChannelImpl::GetQueuedMutex() {
  return IsScheduled() ? scheduler_->GetMutex() : queued_mutex_;
}

bool ChannelImpl::TryDroppingPacket() {
  // Dropping earliest policy is executed
  // TODO implement other policies
  PacketBase* packet = GetNext(true);
  if (packet) Release(packet, true);
  Log(LogEvent<ProfileData::kEventDroppedPacket>(1.0));
  return packet;
}

}  // namespace pipert
