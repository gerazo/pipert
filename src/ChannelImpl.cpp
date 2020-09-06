#include "ChannelImpl.h"

#include <algorithm>
#include <cassert>
#include <mutex>

#include "SchedulerImpl.h"

namespace pipert {

ChannelImpl::ChannelImpl(char* name, int capacity, int packet_size,
                         void* single_thread_object,
                         ChannelBase::InternalCallback callback,
                         SchedulerImpl* scheduler)
    : single_thread_object_(single_thread_object),
      callback_(callback),
      name_(name),
      capacity_(capacity),
      packet_size_(packet_size),
      scheduler_(scheduler),
      queued_(false) {
  pool_ = new int8_t[capacity_ * packet_size_];
  assert(pool_);
  free_packets_.reserve(capacity_);
  for (int i = capacity_ - 1; i >= 0; i--)
    free_packets_.push_back(
        reinterpret_cast<PacketBase*>(pool_ + packet_size_ * i));
  assert((int)free_packets_.size() == capacity_);
  assert((int)free_packets_.capacity() >= capacity_);
  queued_packets_.reserve(capacity_);
  assert(queued_packets_.size() == 0);
  assert((int)queued_packets_.capacity() >= capacity_);
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
  assert(reinterpret_cast<int8_t*>(packet) >= pool_);
  assert(reinterpret_cast<int8_t*>(packet) < pool_ + packet_size_ * capacity_);
  assert((reinterpret_cast<int8_t*>(packet) - pool_) % packet_size_ == 0);
  return packet;
}

void ChannelImpl::Push(PacketBase* packet) {
  // TODO: Record time in packet
  std::lock_guard<AdaptiveSpinLock> lock(queued_mutex_);
  assert((int)queued_packets_.size() < capacity_);
  queued_packets_.push_back(packet);
  std::push_heap(queued_packets_.begin(), queued_packets_.end(),
                 PacketOrdering());
  scheduler_->JobArrived(this);
}

PacketBase* ChannelImpl::GetNext() {
  // TODO: Record time in packet
  PacketBase* top = nullptr;
  std::lock_guard<AdaptiveSpinLock> lock(queued_mutex_);
  assert((int)queued_packets_.size() <= capacity_);
  if (!queued_packets_.empty()) {
    top = queued_packets_.front();
    std::pop_heap(queued_packets_.begin(), queued_packets_.end(),
                  PacketOrdering());
    queued_packets_.pop_back();
  }
  return top;
}

Timer::Time ChannelImpl::PeekNext() const {
  // No locking as it can only be called from Scheduler
  assert(!queued_packets_.empty());
  return queued_packets_.front()->timestamp();
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

bool ChannelImpl::PacketOrdering::operator()(PacketBase* a, PacketBase* b) {
  return a->timestamp() > b->timestamp();
}

bool ChannelImpl::TryDroppingPacket() {
  // TODO: Tell Scheduler about the dropped packet
  // TODO: Implement packet drop policy, now, the earliest will be dropped
  // TODO: Signal dropped packet correctly with times
  PacketBase* packet = GetNext();
  if (packet) Release(packet);
  return packet;
}

void ChannelImpl::Execute() {
  PacketBase* packet = GetNext();
  assert(base_);
  assert(packet);
  callback_(base_, packet);
}

}  // namespace pipert