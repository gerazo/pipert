#ifndef _STATELESS_CHANNEL_IMP_HPP_
#define _STATELESS_CHANNEL_IMP_HPP_

//#include "ChannelImp.h"
#include <mutex>
#include <queue>

namespace pipert {

template <class T>
class ChannelImp;

class SchedulerImp;

template <class T>
class Packet;

template <class T>
class StatelessChannelImp : public ChannelImp<T> {
 public:
  StatelessChannelImp(const std::string& name, SchedulerImp* scheduler,
                      const std::function<void(Packet<T>)>& callback,
                      int buffer_size);

  ~StatelessChannelImp();

  // Execute executes the callback contained in the channel
  // with the freshest element from the buffer
  void Execute() override;

  // Write writes packet into the channel buffer
  // if number of packets less than
  // the buffer_size_, otherwise drop the packet
  // and notify the scheduler that channel is ready for execution
  void Write(const Packet<T>& packet) override;

 private:
  std::queue<Packet<T> > packets_;
  std::mutex m_;
  int dropped_packets_;
};

template <class T>
StatelessChannelImp<T>::StatelessChannelImp(
    const std::string& name, SchedulerImp* scheduler,
    const std::function<void(Packet<T>)>& callback, int buffer_size)
    : ChannelImp<T>(name, scheduler, callback, buffer_size),
      dropped_packets_(0) {}

template <class T>
StatelessChannelImp<T>::~StatelessChannelImp() {
  std::cout << this->name_ << " -> ";
  std::cout << "dropped packets: " << dropped_packets_;
  std::cout << " remained packets is channel: " << packets_.size();
  std::cout << std::endl;
}

template <class T>
void StatelessChannelImp<T>::Execute() {
  m_.lock();
  if (!packets_.empty()) {
    auto packet = packets_.front();
    packets_.pop();
    m_.unlock();
    this->callback_(packet);
  } else {
    m_.unlock();
  }
}

template <class T>
void StatelessChannelImp<T>::Write(const Packet<T>& packet) {
  m_.lock();
  if (packets_.size() < this->buffer_size_) {
    packets_.push(packet);
    m_.unlock();
    this->scheduler_->AddChannel(this);
  } else {
    packets_.pop();
    packets_.push(packet);
    ++dropped_packets_;
    m_.unlock();
  }
}

}  // namespace pipert

#endif  //_STATELESS_CHANNEL_IMP_HPP_