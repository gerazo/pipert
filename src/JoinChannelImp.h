#ifndef _JOIN_CHANNEL_IMP_HPP_
#define _JOIN_CHANNEL_IMP_HPP_

//#include "ChannelBase.h"
#include "pipert/src/IJoinChannel.h"

namespace pipert {

class ChannelBase;

template <class T>
class Packet;

class SchedulerImp;

template <class T1, class T2>
class JoinChannelImp : public ChannelBase, public IJoinChannel<T1, T2> {
 public:
  JoinChannelImp(const std::string& name, SchedulerImp* scheduler,
                 const std::function<void(Packet<T1>, Packet<T2>)>& callback,
                 int buffer_size);

 protected:
  SchedulerImp* scheduler_;
  std::function<void(Packet<T1>, Packet<T2>)> callback_;
};

template <class T1, class T2>
JoinChannelImp<T1, T2>::JoinChannelImp(
    const std::string& name, SchedulerImp* scheduler,
    const std::function<void(Packet<T1>, Packet<T2>)>& callback,
    int buffer_size)
    : ChannelBase(name, buffer_size),
      scheduler_(scheduler),
      callback_(callback) {}

}  // namespace pipert

#endif  //_JOIN_CHANNEL_IMP_HPP_