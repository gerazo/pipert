#ifndef _CHANNEL_BASE_H_
#define _CHANNEL_BASE_H_

#include <string>

namespace pipert {

class ChannelBase {
public:
  ChannelBase(const std::string& name, size_t buffer_size);
  virtual ~ChannelBase();
	virtual void Execute() = 0;

  const std::string& GetName() const { return name_; }
  int GetSize() const { return buffer_size_; }
protected:
  std::string name_;
  size_t buffer_size_;
};

}

#endif //_CHANNEL_BASE_H_