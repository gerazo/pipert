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

int ChannelBase::GetFreePacketSize() {
  return impl_->GetFreePacketSize();
}

int ChannelBase::GetQueuedPacketSize() {
  return impl_->GetQueuedPacketSize();
}

ChannelBase::ChannelBase(ChannelImpl* impl)
  : impl_(impl) {
  assert(impl);
  impl_->SetBase(this); // To be able to call the callback on implementation level
}

ChannelBase::~ChannelBase() {
  if (impl_) {
    delete impl_;
    impl_ = nullptr;
  }
}

PacketBase* ChannelBase::AcquireBase(const char* client_name) {
  assert(impl_);
  return impl_->Acquire(client_name);
}

void ChannelBase::PushBase(PacketBase* packet) {
  assert(impl_);
  impl_->Push(packet);
}

PacketBase* ChannelBase::GetNext() {
  assert(impl_);
  return impl_->GetNext();
}

void ChannelBase::ReleaseBase(PacketBase* packet) {
  assert(impl_);
  impl_->Release(packet);
}

void ChannelBase::move(ChannelBase&& o) {
  assert(o.impl_);
  impl_ = o.impl_;
  impl_->SetBase(this); // same as the case with constructor
  o.impl_ = nullptr;
}

}  // namespace pipert