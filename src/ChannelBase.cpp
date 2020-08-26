#include "pipert/ChannelBase.h"

#include <cassert>
#include <utility>
#include "ChannelImpl.h"

namespace pipert {

ChannelBase::ChannelBase(ChannelBase&& o) {
  move(std::move(o));
}

ChannelBase& ChannelBase::operator=(ChannelBase&& o) {
  move(std::move(o));
  return *this;
}

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

ChannelBase::ChannelBase(ChannelImpl* impl)
  : impl_(impl) {
  assert(impl);
}

ChannelBase::~ChannelBase() {
  if (impl_) {
    delete impl_;
    impl_ = nullptr;
  }
}

PacketBase* ChannelBase::Acquire(const char* client_name) {
  assert(impl_);
  return impl_->Acquire(client_name);
}

void ChannelBase::Push(PacketBase* packet) {
  assert(impl_);
  impl_->Push(packet, this);
}

PacketBase* ChannelBase::GetNext() {
  assert(impl_);
  return impl_->GetNext();
}

void ChannelBase::Release(PacketBase* packet) {
  assert(impl_);
  impl_->Release(packet);
}

void ChannelBase::move(ChannelBase&& o) {
  assert(o.impl_);
  impl_ = o.impl_;
  o.impl_ = nullptr;
}

}  // namespace pipert