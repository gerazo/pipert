#ifndef _MeasurementEvent_H_
#define _MeasurementEvent_H_
#include "pipert/PacketBase.h"

namespace pipert {

class MeasurementEvent {
 public:
  const char* ChannelName;
  Timer::Time PacketTimeStamp;
  Timer::Time AcquireTime;
  Timer::Time PushTime;
  Timer::Time PopTime;
  Timer::Time ExecutionStartTime;
  Timer::Time ExecutionEndTime;
  Timer::Time DroppingTime;
  MeasurementProfileProcessStatus ProcessStatus;

  int* ThreadID;


};      // namespace pipert
#endif  //_MeasurementEvent
}