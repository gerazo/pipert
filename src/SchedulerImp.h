#ifndef _SCHEDULER_IMP_H_
#define _SCHEDULER_IMP_H_

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

#include "ChannelBase.h"
#include "StatefulChannelImp.h"
#include "StatelessChannelImp.h"
#include "pipert/src/IScheduler.h"

namespace pipert {

template <class T>
class Packet;

template <class T>
class Channel;

template <class T>
class StatelessChannelImp;

template <class T>
class StatefulChannelImp;

using StateAndQueue = std::pair<bool, std::queue<ChannelBase*>>;
using StatefulHashtable = std::unordered_map<const void*, StateAndQueue>;

class SchedulerImp : public IScheduler {
 public:
  SchedulerImp(int workers_number);

  ~SchedulerImp();

  template <class T>
  Channel<T> MakeChannel(const std::string& name,
                         const std::function<void(Packet<T>)>& callback,
                         int buffer_size);

  template <class T>
  Channel<T> MakeChannel(const void* mem_address, const std::string& name,
                         const std::function<void(Packet<T>)>& callback,
                         int buffer_size);

  void AddChannel(ChannelBase* channel);

  void InitStatefulChannel(const void* mem_address);
  void AddStatefulChannel(const void* mem_address, ChannelBase* channel);

  void MoveStatefulChannel(const void* mem_address);

  void Start() override;

  void Stop() override;

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

template <class T>
Channel<T> SchedulerImp::MakeChannel(
    const std::string& name, const std::function<void(Packet<T>)>& callback,
    int buffer_size) {
  Channel<T> ch(new StatelessChannelImp<T>(name, this, callback, buffer_size));
  return ch;
}

template <class T>
Channel<T> SchedulerImp::MakeChannel(
    const void* mem_address, const std::string& name,
    const std::function<void(Packet<T>)>& callback, int buffer_size) {
  Channel<T> ch(new StatefulChannelImp<T>(mem_address, name, this, callback,
                                          buffer_size));
  return ch;
}

}  // namespace pipert

#endif  //_SCHEDULER_IMP_H_