#include "pipert/PacketBase.h"
#include "pipert/Timer.h"
#include "pipert/MeasurementProfileBase.h"
namespace pipert {


//this enum define the last level of processing the packet reached in the channel


class MeasurementProfile : MeasurementProfileBase{ 
  const char* MesurementProfileName;
  const char* ChannelName;
  Timer::Time packetTimeStamp;
  Timer::Time AcquireTime;
  Timer::Time PushTime;
  Timer::Time PopTime;
  Timer::Time ExecutionStartTime;
  Timer::Time DroppingTime;
  MeasurementProfileProcessStatus processStatus;

  int* threadID;

	public:
  MeasurementProfile() : MeasurementProfileBase(false) {
    processStatus = NoStatus;
  }


   
};
}  // namespace pipert

