#ifndef _STATEFUL_CHANNEL_IMP_HPP_
#define _STATEFUL_CHANNEL_IMP_HPP_

//#include "ChannelImp.h"
//#include "SchedulerImp.h"
#include <queue>
#include <mutex>

namespace pipert {

template<class T>
class ChannelImp;

template<class T>
class Packet;

class SchedulerImp;

template<class T>
class StatefulChannelImp : public ChannelImp<T> {
public:
  StatefulChannelImp(
        const void* mem_address,
        const std::string& name,
        SchedulerImp* scheduler,
        const std::function<void(Packet<T>)>& callback,
        int buffer_size
      );

  ~StatefulChannelImp();
  
  // Execute executes the callback contained in the channel
  // with the freshest element from the buffer
  void Execute() override;

  // Write writes packet into the channel buffer 
  // if number of packets less than
  // the buffer_size_, otherwise drop the packet
  // and notify the scheduler that channel is ready for execution
  void Write(const Packet<T>& packet) override;

private:
  const void* mem_address_;
  std::queue<Packet<T> > packets_;
  std::mutex m_;
  int dropped_packets_;
};

template<class T>
StatefulChannelImp<T>::StatefulChannelImp(
        const void* mem_address,
        const std::string& name,
        SchedulerImp* scheduler,
        const std::function<void(Packet<T>)>& callback,
        int buffer_size) :
    ChannelImp<T>(name, scheduler, callback, buffer_size),
    mem_address_(mem_address),
    dropped_packets_(0) 
{
  this->scheduler_->InitStatefulChannel(mem_address);
}

template<class T>
StatefulChannelImp<T>::~StatefulChannelImp() 
{
  std::cout << this->name_ << " -> ";
  std::cout << "dropped packets: " << dropped_packets_;
  std::cout << " remained packets is channel: " << packets_.size();
  std::cout << std::endl;
}

template<class T>
void StatefulChannelImp<T>::Execute() 
{
  m_.lock();
  if (!packets_.empty()) {
    auto packet = packets_.front();
    packets_.pop();
    m_.unlock();
    this->callback_(packet);
  } else {
    m_.unlock();
  }
  this->scheduler_->MoveStatefulChannel(mem_address_);
}

template<class T>
void StatefulChannelImp<T>::Write(const Packet<T>& packet) 
{
  m_.lock();
  if (packets_.size() < this->buffer_size_) {
    packets_.push(packet);
    m_.unlock();
    this->scheduler_->AddStatefulChannel(mem_address_, this);
  } else {
    packets_.pop();
    packets_.push(packet);
    ++dropped_packets_;
    m_.unlock();
  }
}

}

#endif //_STATEFUL_CHANNEL_IMP_HPP_