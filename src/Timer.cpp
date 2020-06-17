#include "Timer.h"

namespace pipert {

int64_t Timer::time() 
{
  return (int64_t)std::chrono::duration_cast<std::chrono::microseconds>(
      std::chrono::high_resolution_clock::now().time_since_epoch()).count();
}

void Timer::tic() { beg_ = Clock::now(); }

double Timer::toc() const 
{
  return std::chrono::duration_cast<std::chrono::milliseconds>(
      Clock::now() - beg_).count();
}

int64_t Timer::toc_in_us() const 
{
  return (int64_t)std::chrono::duration_cast<std::chrono::microseconds>(
      Clock::now() - beg_).count();
}

}