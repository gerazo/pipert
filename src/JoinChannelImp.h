#ifndef _JOIN_CHANNEL_IMP_HPP_
#define _JOIN_CHANNEL_IMP_HPP_

//#include "ChannelBase.h"

namespace pipert {

class ChannelBase;

template<class T>
class Packet;

class SchedulerImp;

template<class T1, class T2>
class JoinChannelImp : public ChannelBase {
public:
  JoinChannelImp(
        const std::string& name,
        SchedulerImp* scheduler,
        const std::function<void(Packet<T1>, Packet<T2>)>& callback,
        int buffer_size);

  virtual void WriteFst(const Packet<T1>& packet) = 0;

  virtual void WriteSnd(const Packet<T2>& packet) = 0;

protected:
  SchedulerImp* scheduler_;
  std::function<void(Packet<T1>, Packet<T2>)> callback_;
};

template<class T1, class T2>
JoinChannelImp<T1,T2>::JoinChannelImp(
          const std::string& name,
          SchedulerImp* scheduler,
          const std::function<void(Packet<T1>, Packet<T2>)>& callback,
          int buffer_size) : 
      ChannelBase(name, buffer_size),
      scheduler_(scheduler),
      callback_(callback)
{

} 

}

#endif //_JOIN_CHANNEL_IMP_HPP_