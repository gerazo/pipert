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

ChannelBase::ChannelBase(char* name, int capacity, int packet_size, void* this_mutex, InternalCallback callback) {
  impl_ = new ChannelImpl(name, capacity, packet_size, this_mutex, callback);
}

ChannelBase::~ChannelBase() {
  assert(impl_);
  delete impl_;
  impl_ = nullptr;
}

}  // namespace pipert