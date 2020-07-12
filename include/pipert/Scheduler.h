#ifndef _SCHEDULER_H_
#define _SCHEDULER_H_

#include <functional>
#include "src/IScheduler.h"

namespace pipert {

class Scheduler {
 public:
  Scheduler(int workers);
  ~Scheduler();

  template <class T>
  Channel<T> MakeChannel(const std::string& name,
                         const std::function<void(Packet<T>)>& callback,
                         int buffer_size);

  template <class T>
  Channel<T> MakeChannel(void* mem_address, const std::string& name,
                         const std::function<void(Packet<T>)>& callback,
                         int buffer_size);

  template <class T1, class T2>
  JoinChannel<T1, T2> MakeJoinChannel(
      const std::string& name,
      const std::function<void(Packet<T1>, Packet<T2>)>& callback,
      int buffer_size);

  template <class T1, class T2>
  JoinChannel<T1, T2> MakeJoinChannel(
      const void* mem_address, const std::string& name,
      const std::function<void(Packet<T1>, Packet<T2>)>& callback,
      int buffer_size);

  void Start();
  void Stop();

 private:
  IScheduler* imp_;
};

template <class T>
Channel<T> Scheduler::MakeChannel(
    const std::string& name, const std::function<void(Packet<T>)>& callback,
    int buffer_size) {
  return imp_->MakeChannel(name, callback, buffer_size);
}

template <class T>
Channel<T> Scheduler::MakeChannel(
    void* mem_address, const std::string& name,
    const std::function<void(Packet<T>)>& callback, int buffer_size) {
  return imp_->MakeChannel(mem_address, name, callback, buffer_size);
}

template <class T1, class T2>
JoinChannel<T1, T2> Scheduler::MakeJoinChannel(
    const std::string& name,
    const std::function<void(Packet<T1>, Packet<T2>)>& callback,
    int buffer_size) {
  return imp_->MakeJoinChannel(name, callback, buffer_size);
}

template <class T1, class T2>
JoinChannel<T1, T2> Scheduler::MakeJoinChannel(
    const void* mem_address, const std::string& name,
    const std::function<void(Packet<T1>, Packet<T2>)>& callback,
    int buffer_size) {
  return imp_->MakeJoinChannel(mem_address, name, callback, buffer_size);
}

}  // namespace pipert

#endif  //_SCHEDULER_H_