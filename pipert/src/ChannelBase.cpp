#include "ChannelBase.h"

namespace pipert {

ChannelBase::ChannelBase(const std::string& name, size_t buffer_size) : 
        name_(name),
        buffer_size_(buffer_size) 
{
  
}

ChannelBase::~ChannelBase() = default;

}