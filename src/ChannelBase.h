#ifndef _CHANNEL_BASE_H_
#define _CHANNEL_BASE_H_

#include <string>
#include "pipert/src/IChannel.h"

namespace pipert {

class ChannelBase : public IChannelBase {
 public:
  ChannelBase(const std::string& name, size_t buffer_size);
  virtual ~ChannelBase();

  const std::string& GetName() const override { return name_; }
  int GetSize() const override { return buffer_size_; }

 protected:
  std::string name_;
  size_t buffer_size_;
};

}  // namespace pipert

#endif  //_CHANNEL_BASE_H_