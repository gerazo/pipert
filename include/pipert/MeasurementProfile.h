#ifndef _MeasurementProfile_H_
#define _MeasurementProfile_H_
#include "pipert/MeasurementProfileBase.h"
#include "pipert/PacketBase.h"
#include "pipert/Timer.h"
#include "pipert/MeasurementEvent.h"
namespace pipert {

class MeasurementProfile : MeasurementProfileBase {



 public:
  const char* MesurementProfileName;
  std::vector<MeasurementEvent> MeasurementsEventsLog;
  MeasurementProfile() : MeasurementProfileBase(false) {

  }
};
}  // namespace pipert
  // namespace pipert
#endif  //_PMeasurementProfileBase_H_
