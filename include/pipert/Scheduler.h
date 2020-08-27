#ifndef _SCHEDULER_H_
#define _SCHEDULER_H_

#include <functional>
#include "pipert/PolledChannel.h"
#include "pipert/ScheduledChannel.h"

namespace pipert {

class SchedulerImpl;

class Scheduler {
 public:
  Scheduler(int workers);  ///< Entering preparation state
  ~Scheduler();

  Scheduler(const Scheduler&) = delete;
  Scheduler& operator=(const Scheduler&) = delete;

  template <class T>
  PolledChannel<T> CreatePolledChannel(char* name, int capacity);

  template <class T>
  ScheduledChannel<T> CreateScheduledChannel(
      char* name, int capacity, void* single_thread_object,
      typename ScheduledChannel<T>::Callback callback);

  void Start();  ///< Entering running state
  void Stop();

 private:
  SchedulerImpl* impl_;

  ChannelImpl* CreateChannelImpl(char* name, int capacity, int packet_size,
                                 void* single_thread_object,
                                 ChannelBase::InternalCallback callback);
};

template <class T>
PolledChannel<T> Scheduler::CreatePolledChannel(char* name, int capacity) {
  ChannelImpl* chimpl =
      CreateChannelImpl(name, capacity, sizeof(Packet<T>), nullptr, nullptr);
  return PolledChannel<T>(chimpl);
}

template <class T>
ScheduledChannel<T> Scheduler::CreateScheduledChannel(
    char* name, int capacity, void* single_thread_object,
    typename ScheduledChannel<T>::Callback callback) {
  ChannelImpl* chimpl =
      CreateChannelImpl(name, capacity, sizeof(Packet<T>), single_thread_object,
                        &ScheduledChannel<T>::CallbackTranslator);
  return ScheduledChannel<T>(chimpl, callback);
}

}  // namespace pipert

#endif  //_SCHEDULER_H_