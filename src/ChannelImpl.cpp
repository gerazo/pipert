#include "ChannelImpl.h"

#include <algorithm>
#include <cassert>
#include <mutex>

namespace pipert {

ChannelImpl::ChannelImpl(char* name, int capacity, int packet_size, void* mutex_state, ChannelBase::InternalCallback callback)
  : mutex_state_(mutex_state), callback_(callback), name_(name), capacity_(capacity), packet_size_(packet_size) {
  pool_ = new int8_t[capacity_ * packet_size_];
  free_packets_.reserve(capacity_);
  for (int i = capacity_ - 1; i >= 0; i--) free_packets_.push_back(i);
  assert((int)free_packets_.size() == capacity_);
  assert((int)free_packets_.capacity() >= capacity_);
  queued_packets_.reserve(capacity_);
  assert(queued_packets_.size() == 0);
  assert((int)queued_packets_.capacity() >= capacity_);
}

ChannelImpl::~ChannelImpl() {
  assert(pool_);
  delete[] reinterpret_cast<int8_t*>(pool_);
  pool_ = nullptr;
}

PacketBase* ChannelImpl::Acquire(const char* client_name) {
  (void)client_name;
  // TODO: Record acquirer and time in packet
  int free_idx = -1;
  {
    std::lock_guard<AdaptiveSpinLock> lock(free_mutex_);
    if (!free_packets_.empty()) {
      free_idx = free_packets_.back();
      free_packets_.pop_back();
    }
  }
  if (free_idx == -1) {
    // TODO: Implement packet drop policy, now, the earliest will be dropped
    // TODO: Signal dropped packet correctly with times
    Release(GetNext());
    std::lock_guard<AdaptiveSpinLock> lock(free_mutex_);
    assert(!free_packets_.empty());
    free_idx = free_packets_.back();
    free_packets_.pop_back();
  }
  assert(free_idx >= 0 && free_idx < capacity_);
  return reinterpret_cast<PacketBase*>(pool_ + packet_size_ * free_idx);
}

void ChannelImpl::Push(PacketBase* packet) {
  // TODO: Record time in packet
  std::lock_guard<AdaptiveSpinLock> lock(queued_mutex_);
  assert((int)queued_packets_.size() < capacity_);
  queued_packets_.push_back(packet);
  std::push_heap(queued_packets_.begin(), queued_packets_.end(), PacketOrdering());
}

PacketBase* ChannelImpl::GetNext() {
  // TODO: Record time in packet
  std::lock_guard<AdaptiveSpinLock> lock(queued_mutex_);
  assert(!queued_packets_.empty());
  assert((int)queued_packets_.size() <= capacity_);
  PacketBase* top = queued_packets_.front();
  std::pop_heap(queued_packets_.begin(), queued_packets_.end(), PacketOrdering());
  queued_packets_.pop_back();
  return top;
}

void ChannelImpl::Release(PacketBase* packet) {
  // TODO: Record this time and all data from packet
  int8_t* address = reinterpret_cast<int8_t*>(packet);
  assert(address >= pool_ && address < pool_ + packet_size_ * capacity_);
  assert((address - pool_) % packet_size_ == 0);
  int idx = (address - pool_) / packet_size_;
  std::lock_guard<AdaptiveSpinLock> lock(free_mutex_);
  assert((int)free_packets_.size() < capacity_);
  free_packets_.push_back(idx);
}

}  // namespace pipert