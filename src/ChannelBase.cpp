#include "pipert/ChannelBase.h"

#include <cassert>
#include "ChannelImpl.h"

namespace pipert {

int ChannelBase::GetCapacity() const {
  assert(impl_);
  return impl_->GetCapacity();
}

int ChannelBase::GetPacketSize() const {
  assert(impl_);
  return impl_->GetPacketSize();
}

const char* ChannelBase::GetName() const {
  assert(impl_);
  return impl_->GetName();
}

ChannelBase::ChannelBase(char* name, int capacity, int packet_size, void* mutex_state, InternalCallback callback) {
  impl_ = new ChannelImpl(name, capacity, packet_size, mutex_state, callback, nullptr);
}

ChannelBase::ChannelBase(ChannelImpl* impl)
  : impl_(impl) {}

ChannelBase::~ChannelBase() {
  assert(impl_);
  delete impl_;
  impl_ = nullptr;
}

PacketBase* ChannelBase::Acquire(const char* client_name) {
  return impl_->Acquire(client_name);
}

void ChannelBase::Push(PacketBase* packet) {
  impl_->Push(packet, this);
}

PacketBase* ChannelBase::GetNext() {
  return impl_->GetNext();
}

void ChannelBase::Release(PacketBase* packet) {
  impl_->Release(packet);
}

}  // namespace pipert