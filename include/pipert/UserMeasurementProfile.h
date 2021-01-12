#ifndef _UserMeasurementProfile_H_
#define _UserMeasurementProfile_H_

#include "pipert/MeasurementEvent.h"
#include "pipert/MeasurementProfileBase.h"
#include "pipert/Packet.h"
#include "pipert/PacketBase.h"
#include "pipert/Timer.h"
#include "pipert/UserMeasurementProfile.h"
#include <iostream>
using namespace std;

namespace pipert {
template <class T>
class UserMeasurementProfile : public pipert::MeasurementProfileBase {
  typedef std::map<std::string, std::string> (*FuncPtrStringInt)(
      T packet_data_);

 public:
  const char *MesurementProfileName;
  std::vector<MeasurementEvent> MeasurementsEventsLog;
  std::map<std::string, std::string> userCallBackFunctionResult;
  FuncPtrStringInt MeasurementProcessingFunction;


  std::map<std::string, string> getResult() {
    return userCallBackFunctionResult;
  }

  UserMeasurementProfile<T>(FuncPtrStringInt measurementProcessingFunction,T data)
      : MeasurementProfileBase(true) {
    MeasurementProcessingFunction = measurementProcessingFunction;
    userCallBackFunctionResult=measurementProcessingFunction(data);

  }

};
} // namespace pipert

#endif