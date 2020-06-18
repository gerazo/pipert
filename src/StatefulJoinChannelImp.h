#ifndef _STATEFUL_JOIN_CHANNEL_IMP_HPP_
#define _STATEFUL_JOIN_CHANNEL_IMP_HPP_

#include <functional>
#include <iostream>
#include <mutex>
#include <queue>

//#include "JoinChannelImp.h"
//#include "SchedulerImp.h"

namespace pipert {

template <class T>
class Packet;

class SchedulerImp;

template <class T1, class T2>
class JoinChannelImp;

template <class T1, class T2>
class StatefulJoinChannelImp : public JoinChannelImp<T1, T2> {
 public:
  StatefulJoinChannelImp(
      const void* mem_adress, const std::string& name, SchedulerImp* scheduler,
      const std::function<void(Packet<T1>, Packet<T2>)>& callback,
      int buffer_size);

  void Execute() override;

  void WriteFst(const Packet<T1>& packet) override;

  void WriteSnd(const Packet<T2>& packet) override;

 private:
  const void* mem_address_;
  std::queue<Packet<T1>> fst_packets_;
  std::queue<Packet<T2>> snd_packets_;
  std::mutex m_;
  int dropped_packets_;
};

template <class T1, class T2>
StatefulJoinChannelImp<T1, T2>::StatefulJoinChannelImp(
    const void* mem_adress, const std::string& name, SchedulerImp* scheduler,
    const std::function<void(Packet<T1>, Packet<T2>)>& callback,
    int buffer_size)
    : JoinChannelImp<T1, T2>(name, scheduler, callback, buffer_size),
      mem_address_(mem_adress),
      dropped_packets_(0) {
  this->scheduler_->InitStatefulChannel(mem_address_);
}

template <class T1, class T2>
void StatefulJoinChannelImp<T1, T2>::Execute() {
  m_.lock();
  // std::cout << "Execute was called" << std::endl;
  if (!fst_packets_.empty() && !snd_packets_.empty()) {
    auto fst_packet = fst_packets_.front();
    auto snd_packet = snd_packets_.front();
    fst_packets_.pop();
    snd_packets_.pop();
    m_.unlock();
    this->callback_(fst_packet, snd_packet);
  } else {
    m_.unlock();
  }
  this->scheduler_->MoveStatefulChannel(mem_address_);
}

template <class T1, class T2>
void StatefulJoinChannelImp<T1, T2>::WriteFst(const Packet<T1>& packet) {
  m_.lock();
  if (fst_packets_.size() < this->buffer_size_) {
    fst_packets_.push(packet);
    m_.unlock();
    this->scheduler_->AddStatefulChannel(mem_address_, this);
  } else {
    fst_packets_.pop();
    fst_packets_.push(packet);
    ++dropped_packets_;
    m_.unlock();
  }
}

template <class T1, class T2>
void StatefulJoinChannelImp<T1, T2>::WriteSnd(const Packet<T2>& packet) {
  m_.lock();
  if (snd_packets_.size() < this->buffer_size_) {
    snd_packets_.push(packet);
    m_.unlock();
    this->scheduler_->AddStatefulChannel(mem_address_, this);
  } else {
    snd_packets_.pop();
    snd_packets_.push(packet);
    ++dropped_packets_;
    m_.unlock();
  }
}

}  // namespace pipert

#endif  //_STATEFUL_JOIN_CHANNEL_IMP_HPP_