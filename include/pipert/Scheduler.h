#ifndef _SCHEDULER_H_
#define _SCHEDULER_H_

#include <functional>
#include "pipert/PolledChannel.h"
#include "pipert/ScheduledChannel.h"

namespace pipert {

class SchedulerImpl;

class Scheduler {
 public:
  Scheduler(int workers);
  ~Scheduler();

  template <class T>
  PolledChannel<T> CreatePolledChannel(char* name, int capacity);

  template <class T>
  ScheduledChannel<T> CreateScheduledChannel(char* name, int capacity,
      void* mutex_state, typename ScheduledChannel<T>::Callback callback);

  void Start();
  void Stop();

 private:
  SchedulerImpl* impl_;

  ChannelImpl* CreateChannelImpl(char* name, int capacity, int packet_size,
    void* mutex_state, ChannelBase::InternalCallback callback);
};

template <class T>
PolledChannel<T> Scheduler::CreatePolledChannel(char* name, int capacity) {
  ChannelImpl* chimpl = CreateChannelImpl(name, capacity, sizeof(Packet<T>),
                                          nullptr, nullptr);
  return PolledChannel<T>(chimpl);
}

template <class T>
ScheduledChannel<T> Scheduler::CreateScheduledChannel(char* name, int capacity,
    void* mutex_state, typename ScheduledChannel<T>::Callback callback) {
  ChannelImpl* chimpl = CreateChannelImpl(name, capacity, sizeof(Packet<T>),
    mutex_state, &ScheduledChannel<T>::CallbackTranslator);
  return ScheduledChannel<T>(chimpl, callback);
}

}  // namespace pipert

#endif  //_SCHEDULER_H_