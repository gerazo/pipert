#ifndef _ICHANNEL_H_
#define _ICHANNEL_H_

#include <string>

namespace pipert {

class IChannelBase {
public:
  virtual void Execute() = 0;
  virtual const std::string& GetName() const = 0;
  virtual int GetSize() const = 0;
};

template<class T>
class Packet;

template<class T>
class IChannel : IChannelBase {
public:
  virtual void Write(const Packet<T>& packet) = 0;
private:
};

} // namespace pipert

#endif //_ICHANNEL_H_