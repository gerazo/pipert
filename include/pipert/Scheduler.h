#ifndef _SCHEDULER_H_
#define _SCHEDULER_H_

#include <functional>
#include "pipert/SchedulerImpl.h"
#include "pipert/PolledChannel.h"
#include "pipert/ScheduledChannel.h"

namespace pipert {

class Scheduler {
 public:
  Scheduler(int workers);
  ~Scheduler();

  template <class T>
  PolledChannel<T> MakePolledChannel(char* name,
                                  int capacity);

  template <class T>
  ScheduledChannel<T> MakeScheduledChannel(char* name,
                          int capacity,
                          void* mutex_state,
                          const std::function<void(PacketToProcess<T>)>& callback);

  void Start();
  void Stop();

 private:
  SchedulerImpl* impl_;
};

template <class T>
PolledChannel<T> Scheduler::MakePolledChannel(char* name,
                                int capacity) {
  return  PolledChannel<T>(impl_->MakePolledChannel(name, capacity, sizeof(Packet<T>)));
}

template <class T>
ScheduledChannel<T> Scheduler::MakeScheduledChannel(char* name,
                          int capacity,
                          void* mutex_state,
                          const std::function<void(PacketToProcess<T>)>& callback) {
  return ScheduledChannel<T>(impl_->MakeScheduledChannel(name, capacity, sizeof(Packet<T>), mutex_state, &ScheduledChannel<T>::CallbackTranslator));
}

}  // namespace pipert

#endif  //_SCHEDULER_H_