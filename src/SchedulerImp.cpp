#include "SchedulerImp.h"

namespace pipert {

SchedulerImp::SchedulerImp(int workers_number) : 
        workers_number_(workers_number) 
{
  workers_.reserve(workers_number_);
}

SchedulerImp::~SchedulerImp()
{
  Stop();
}

void SchedulerImp::AddChannel(ChannelBase* channel) 
{
  std::lock_guard<std::mutex> guard(m_);
  channels_.push_back(channel);
}

void SchedulerImp::InitStatefulChannel(const void* mem_address) 
{
  std::lock_guard<std::mutex> guard(m_);
  stateful_channels_hash_.insert({mem_address, {true, {}}});
}

void SchedulerImp::AddStatefulChannel(const void* mem_address, ChannelBase* channel)
{
  m_.lock();
  StateAndQueue& state_and_queue = stateful_channels_hash_.find(mem_address)->second;
  //assert if find function gives end

  state_and_queue.second.push(channel);

  if (state_and_queue.first) {
    state_and_queue.first = false;
    channels_.push_front(state_and_queue.second.front());
    state_and_queue.second.pop();
    m_.unlock();
  } else {
    m_.unlock();
  }
}

void SchedulerImp::MoveStatefulChannel(const void* mem_address) {
  std::lock_guard<std::mutex> guard(m_);
  StateAndQueue& state_and_queue = stateful_channels_hash_.find(mem_address)->second;
  if (state_and_queue.second.empty()) {
    state_and_queue.first = true;
  } else {
    channels_.push_front(state_and_queue.second.front());
    state_and_queue.second.pop();
  }
}

void SchedulerImp::Start() 
{
  //worker = std::thread([](){ std::cout << "Thread started" << std::endl; });
  keep_running_.store(true, std::memory_order_release);
  while (workers_number_-- > 0) {
    std::thread t = std::thread(&SchedulerImp::RunTasks, this);
    t.detach();
    workers_.push_back(&t);
  }
}

void SchedulerImp::Stop() 
{
  //std::lock_guard<std::mutex> guard(m);
  std::cout << "Stop was called" << std::endl;
  keep_running_.store(false, std::memory_order_release);
}

void SchedulerImp::RunTasks() 
{
  while (keep_running_.load(std::memory_order_acquire)) {
    m_.lock();
    if (!channels_.empty()) {
      ChannelBase* ch = channels_.front();
      channels_.pop_front();
      m_.unlock();
      ch->Execute();
    } else {
      m_.unlock();
    }
  }
}


}