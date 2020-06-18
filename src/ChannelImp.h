#ifndef _CHANNEL_IMP_HPP_
#define _CHANNEL_IMP_HPP_

#include <functional>
#include <iostream>
#include "ChannelBase.h"

namespace pipert {

class ChannelBase;

template <class T>
class Packet;

class SchedulerImp;

template <class T>
class ChannelImp : public ChannelBase {
 public:
  ChannelImp(const std::string& name, SchedulerImp* scheduler,
             const std::function<void(Packet<T>)>& callback, int buffer_size);

  virtual void Write(const Packet<T>& packet) = 0;

 protected:
  SchedulerImp* scheduler_;
  std::function<void(Packet<T>)> callback_;
};

template <class T>
ChannelImp<T>::ChannelImp(const std::string& name, SchedulerImp* scheduler,
                          const std::function<void(Packet<T>)>& callback,
                          int buffer_size)
    : ChannelBase(name, buffer_size),
      scheduler_(scheduler),
      callback_(callback) {}

}  // namespace pipert

#endif  //_CHANNEL_IMP_HPP_