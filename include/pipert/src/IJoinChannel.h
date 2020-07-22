#ifndef _IJOINCHANNEL_H_
#define _IJOINCHANNEL_H_

#include <string>
#include "IChannel.h"

namespace pipert {

template<class T>
class Packet;

template<class T1, class T2>
class IJoinChannel : IChannelBase {
public:
  virtual void WriteFst(const Packet<T1>& packet) = 0;
  virtual void WriteSnd(const Packet<T2>& packet) = 0;
private:
};

} // namespace pipert

#endif //_ICHANNEL_H_