#include "pipert/MeasurementProfileBase.h"
#include "pipert/PacketBase.h"
#include "pipert/Timer.h"
namespace pipert {

class MeasurementProfile : MeasurementProfileBase {
 private:
  const char* MesurementProfileName;
  const char* ChannelName;
  Timer::Time PacketTimeStamp;
  Timer::Time AcquireTime;
  Timer::Time PushTime;
  Timer::Time PopTime;
  Timer::Time ExecutionStartTime;
  Timer::Time DroppingTime;
  MeasurementProfileProcessStatus ProcessStatus;

  int* ThreadID;

 public:
  MeasurementProfile() : MeasurementProfileBase(false) {
    ProcessStatus = NoStatus;
  }
};
}  // namespace pipert
