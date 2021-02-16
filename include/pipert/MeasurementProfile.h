#ifndef _MeasurementProfile_H_
#define _MeasurementProfile_H_
#include <string>
#include <vector>

#include "MeasurementEvent.h"
#include "pipert/MeasurementProfileBase.h"
#include "pipert/PacketBase.h"
#include "pipert/Timer.h"

namespace pipert {

class MeasurementProfile : public MeasurementProfileBase {
 public:
  std::string MesurementProfileName;
  std::vector<MeasurementEvent> MeasurementsEventsLog;
  MeasurementProfile() : MeasurementProfileBase(false) {

  }

};
}  // namespace pipert
  // namespace pipert
#endif  //_PMeasurementProfileBase_H_
