#ifndef _SCHEDULER_IMPL_H_
#define _SCHEDULER_IMPL_H_

#include <unistd.h>
#include <atomic>
#include <deque>
#include <functional>
#include <iostream>
#include <mutex>
#include <queue>
#include <thread>
#include <unordered_map>
#include <vector>

#include "pipert/ChannelBase.h"

namespace pipert {

template <class T>
class Packet;

template <class T>
class Channel;

using StateAndQueue = std::pair<bool, std::queue<ChannelBase*>>;
using StatefulHashtable = std::unordered_map<const void*, StateAndQueue>;

class SchedulerImpl {
 public:
  using InternalCallback = void (*)(ChannelBase*, PacketBase*);

  SchedulerImpl(int workers_number);

  ~SchedulerImpl();

  ChannelImpl* MakePolledChannel(char* name,
                          int capacity,
                          int packet_size);

  ChannelImpl* MakeScheduledChannel(char* name,
                          int capacity,
                          int packet_size,
                          void* mutex_state,
                          InternalCallback callback);

  void AddChannel(ChannelBase* channel);

  void InitStatefulChannel(const void* mem_address);
  void AddStatefulChannel(const void* mem_address, ChannelBase* channel);

  void MoveStatefulChannel(const void* mem_address);

  void Start();

  void Stop();

 private:
  std::deque<ChannelBase*> channels_;
  // StateAndQueue statefulChannels;
  StatefulHashtable stateful_channels_hash_;
  std::mutex m_;
  std::vector<std::thread*> workers_;
  std::atomic_bool keep_running_;
  int workers_number_;

  void RunTasks();
};

}  // namespace pipert

#endif  //_SCHEDULER_IMPL_H_