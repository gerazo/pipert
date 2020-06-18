#ifndef _STATELESS_JOIN_CHANNEL_IMP_HPP_
#define _STATELESS_JOIN_CHANNEL_IMP_HPP_

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
class StatelessJoinChannelImp : public JoinChannelImp<T1, T2> {
 public:
  StatelessJoinChannelImp(
      const std::string& name, SchedulerImp* scheduler,
      const std::function<void(Packet<T1>, Packet<T2>)>& callback,
      int buffer_size);

  void Execute() override;

  void WriteFst(const Packet<T1>& packet) override;

  void WriteSnd(const Packet<T2>& packet) override;

 private:
  std::queue<Packet<T1>> fst_packets_;
  std::queue<Packet<T2>> snd_packets_;
  std::mutex m_;
  int dropped_packets_;
};

template <class T1, class T2>
StatelessJoinChannelImp<T1, T2>::StatelessJoinChannelImp(
    const std::string& name, SchedulerImp* scheduler,
    const std::function<void(Packet<T1>, Packet<T2>)>& callback,
    int buffer_size)
    : JoinChannelImp<T1, T2>(name, scheduler, callback, buffer_size),
      dropped_packets_(0) {}

template <class T1, class T2>
void StatelessJoinChannelImp<T1, T2>::Execute() {
  m_.lock();
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
}

template <class T1, class T2>
void StatelessJoinChannelImp<T1, T2>::WriteFst(const Packet<T1>& packet) {
  m_.lock();
  if (fst_packets_.size() < this->buffer_size_) {
    fst_packets_.push(packet);
    m_.unlock();
    this->scheduler_->AddChannel(this);
  } else {
    fst_packets_.pop();
    fst_packets_.push(packet);
    ++dropped_packets_;
    m_.unlock();
  }
}

template <class T1, class T2>
void StatelessJoinChannelImp<T1, T2>::WriteSnd(const Packet<T2>& packet) {
  m_.lock();
  if (snd_packets_.size() < this->buffer_size_) {
    snd_packets_.push(packet);
    m_.unlock();
    this->scheduler_->AddChannel(this);
  } else {
    snd_packets_.pop();
    snd_packets_.push(packet);
    ++dropped_packets_;
    m_.unlock();
  }
}

}  // namespace pipert

#endif  //_STATELESS_JOIN_CHANNEL_IMP_HPP_