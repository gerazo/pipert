#include "ChannelImpl.h"

namespace pipert {

ChannelImpl::ChannelImpl(char* name, int capacity, int packet_size, void* this_mutex, ChannelBase::InternalCallback callback)
  : this_mutex_(this_mutex), callback_(callback), name_(name), capacity_(capacity), packet_size_(packet_size) {

}

ChannelImpl::~ChannelImpl() {

}

}  // namespace pipert