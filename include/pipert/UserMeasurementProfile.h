#ifndef _UserMeasurementProfile_H_
#define _UserMeasurementProfile_H_

#include "pipert/MeasurementProfileBase.h"
#include "pipert/PacketBase.h"
#include "pipert/Timer.h"
#include "pipert/MeasurementEvent.h"
#include "pipert/Packet.h"
#include "pipert/UserMeasurementProfile.h"
#include <iostream>

namespace pipert {
template <class T>
class UserMeasurementProfile :pipert::MeasurementProfileBase {
  typedef std::map<std::string, std::string> (*FuncPtrStringInt)(T packet_data_);

 public:
  const char* MesurementProfileName;
  std::vector<MeasurementEvent> MeasurementsEventsLog;
  std::map<std::string, std::string> userCallBackFunctionResult;
  FuncPtrStringInt MeasurementProcessingFunction;
  T packet_data_;

  std::map<std::string, string>  getResult(){
 return MeasurementProcessingFunction(packet_data_);
  }

  UserMeasurementProfile<T>(T packetData,FuncPtrStringInt measurementProcessingFunction):MeasurementProfileBase(true){
     MeasurementProcessingFunction = measurementProcessingFunction;
    packet_data_=packetData;
  }
};
}

#endif