#include "ChannelImpl.h"

#include <algorithm>
#include <cassert>
#include <mutex>

#include "SchedulerImpl.h"

namespace pipert {

ChannelImpl::ChannelImpl(const char* name, int capacity, int packet_size,
                         void* single_thread_object,
                         ChannelBase::InternalCallback callback,
                         SchedulerImpl* scheduler)
    : single_thread_object_(single_thread_object),
      callback_(callback),
      name_(name),
      capacity_(capacity),
      packet_size_(packet_size),
      scheduler_(scheduler),
      base_(nullptr) {
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

PacketBase* ChannelImpl::Acquire(const char* client_name) {
  (void)client_name;
  // TODO: Record acquirer and time in packet
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
  return packet;
}

void ChannelImpl::Push(PacketBase* packet) {
  // TODO: Record time in packet
  std::lock_guard<AdaptiveSpinLock> lock(GetQueuedMutex());
  assert((int)queued_packets_.size() < capacity_);
  Timer::Time earliest = queued_packets_.empty() ? Timer::kMaxTime : PeekNext();
  queued_packets_.push_back(packet);
  std::push_heap(queued_packets_.begin(), queued_packets_.end(),
                 PacketOrdering());
  if (IsScheduled() && PeekNext() < earliest) {
    if (earliest == Timer::kMaxTime)
      scheduler_->JobsArrived(this);
    else
      scheduler_->JobsUpdated(this, true);
  }
}

PacketBase* ChannelImpl::GetNext() {
  std::lock_guard<AdaptiveSpinLock> lock(GetQueuedMutex());
  return !queued_packets_.empty() ? PopNext() : nullptr;
}

void ChannelImpl::Release(PacketBase* packet) {
  // TODO: Record this time and all data from packet
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

PacketBase* ChannelImpl::PopNext() {
  // No locking as it is called from Scheduler or here
  // TODO: Record time in packet
  assert((int)queued_packets_.size() <= capacity_);
  assert(!queued_packets_.empty());
  PacketBase* earliest = queued_packets_.front();
  std::pop_heap(queued_packets_.begin(), queued_packets_.end(),
                PacketOrdering());
  queued_packets_.pop_back();
  if (IsScheduled()) {
    if (queued_packets_.empty())
      scheduler_->JobsDropped(this);
    else if (earliest != queued_packets_.front())
      scheduler_->JobsUpdated(this, false);
  }
  return earliest;
}

void ChannelImpl::Execute(PacketBase* packet) {
  // TODO: Record time
  assert(base_);
  assert(packet);
  callback_(base_, packet);
}

int ChannelImpl::GetFreeBufferLength() {
  std::lock_guard<AdaptiveSpinLock> lock(free_mutex_);
  return free_packets_.size();
}

int ChannelImpl::GetQueuedBufferLength() {
  std::lock_guard<AdaptiveSpinLock> lock(GetQueuedMutex());
  return queued_packets_.size();
}

bool ChannelImpl::PacketOrdering::operator()(PacketBase* a, PacketBase* b) {
  return a->timestamp() > b->timestamp();
}

void ChannelImpl::SetBase(ChannelBase* base) { base_ = base; }

AdaptiveSpinLock& ChannelImpl::GetQueuedMutex() {
  return IsScheduled() ? scheduler_->GetMutex() : queued_mutex_;
}

bool ChannelImpl::TryDroppingPacket() {
  // Dropping earliest policy is executed
  // TODO: Signal dropped packet correctly with times
  PacketBase* packet = GetNext();
  if (packet) Release(packet);
  return packet;
}

}  // namespace pipert