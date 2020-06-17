#ifndef _SCHEDULER_IMP_H_
#define _SCHEDULER_IMP_H_

#include <queue>
#include <deque>
#include <functional>
#include <thread>
#include <atomic>
#include <iostream>
#include <vector>
#include <mutex>
#include <unistd.h>
#include <unordered_map>

#include "ChannelBase.h"
#include "StatefulChannelImp.h"
#include "StatelessChannelImp.h"
#include "StatelessJoinChannelImp.h"
#include "StatefulJoinChannelImp.h"

namespace pipert {

template<class T>
class Packet;

template<class T>
class Channel;

template<class T>
class StatelessChannelImp;

template<class T>
class StatefulChannelImp;

template<class T1, class T2>
class JoinChannel;

template<class T1, class T2>
class StatelessJoinChannelImp;

template<class T1, class T2>
class StatefulJoinChannelImp;

using StateAndQueue = std::pair<bool,std::queue<ChannelBase*>>;
using StatefulHashtable = std::unordered_map<const void*, StateAndQueue>;

class SchedulerImp {
public:
  SchedulerImp(int workers_number);

  ~SchedulerImp();

  template<class T>
  Channel<T> MakeChannel(
          const std::string& name,
          const std::function<void(Packet<T>)>& callback,
          int buffer_size);

  template<class T>
  Channel<T> MakeChannel(
          const void* mem_address,
          const std::string& name,
          const std::function<void(Packet<T>)>& callback,
          int buffer_size);

  template<class T1, class T2>
  JoinChannel<T1, T2> MakeJoinChannel(
          const std::string& name,
          const std::function<void(Packet<T1>, Packet<T2>)>& callback,
          int buffer_size);

  template<class T1, class T2>
  JoinChannel<T1, T2> MakeJoinChannel(
          const void* mem_address,
          const std::string& name,
          const std::function<void(Packet<T1>, Packet<T2>)>& callback,
          int buffer_size);

  void AddChannel(ChannelBase* channel);

  void InitStatefulChannel(const void* mem_address);
  void AddStatefulChannel(const void* mem_address, ChannelBase* channel);

  void MoveStatefulChannel(const void* mem_address);

  void Start();

  void Stop();

private:
  std::deque<ChannelBase*> channels_;
  //StateAndQueue statefulChannels;
  StatefulHashtable stateful_channels_hash_;
  std::mutex m_;
  std::vector<std::thread*> workers_;
  std::atomic_bool keep_running_;
  int workers_number_;

  void RunTasks();
};

template<class T>
Channel<T> SchedulerImp::MakeChannel(
        const std::string& name,
        const std::function<void(Packet<T>)>& callback,
        int buffer_size)
{
  Channel<T> ch(new StatelessChannelImp<T>(name, this, callback, buffer_size));
  return ch;
}

template<class T>
Channel<T> SchedulerImp::MakeChannel(
        const void* mem_address,
        const std::string& name,
        const std::function<void(Packet<T>)>& callback,
        int buffer_size)
{
  Channel<T> ch(new StatefulChannelImp<T>(mem_address, name, this, callback, buffer_size));
  return ch;
}

template<class T1, class T2>
JoinChannel<T1, T2> SchedulerImp::MakeJoinChannel(
        const std::string& name,
        const std::function<void(Packet<T1>, Packet<T2>)>& callback,
        int buffer_size)
{
  JoinChannel<T1, T2> ch(new StatelessJoinChannelImp<T1, T2>(name, this, callback, buffer_size));
  return ch;
}

template<class T1, class T2>
JoinChannel<T1, T2> SchedulerImp::MakeJoinChannel(
        const void* mem_address,
        const std::string& name,
        const std::function<void(Packet<T1>, Packet<T2>)>& callback,
        int buffer_size)
{
  JoinChannel<T1, T2> ch(new StatefulJoinChannelImp<T1, T2>(mem_address, name, this, callback, buffer_size));
  return ch;
}

}

#endif //_SCHEDULER_IMP_H_