#ifndef _SCHEDULER_H_
#define _SCHEDULER_H_

#include <functional>
#include "pipert/SchedulerImp.h"

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

  void Start();
  void Stop();

 private:
  SchedulerImp* imp_;
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

}  // namespace pipert

#endif  //_SCHEDULER_H_