#ifndef _UserMeasurementProfile_H_
#define _UserMeasurementProfile_H_

#include "pipert/MeasurementEvent.h"
#include "pipert/MeasurementProfileBase.h"
#include "pipert/Timer.h"
#include <iostream>
#include<map>
#include<vector>
using namespace std;

namespace pipert {
template<class T>
class UserMeasurementProfile : public pipert::MeasurementProfileBase {

  typedef std::map<std::string, std::string> (*FuncPtrStringInt)(
      T packet_data_);

 public:
  const char *MesurementProfileName;
  std::vector<MeasurementEvent> MeasurementsEventsLog;
  std::map<std::string, std::string> userCallBackFunctionResult;
  FuncPtrStringInt MeasurementProcessingFunction;
  bool measurement_function_is_set_;
  bool activated_;


  std::map<std::string, string> getResult() {
    return userCallBackFunctionResult;
  }
  std::map<std::string, string> calculate(T data) {
    if(activated_ && measurement_function_is_set_)
    return measurementProcessingFunction(data);
  }


  UserMeasurementProfile<T>(FuncPtrStringInt measurementProcessingFunction,T data)
      : MeasurementProfileBase(true) {
    MeasurementProcessingFunction = measurementProcessingFunction;
    userCallBackFunctionResult=measurementProcessingFunction(data);

  }
  UserMeasurementProfile<T>(): MeasurementProfileBase(true) {
    measurement_function_is_set_= false;
    activated_=false;
                                };

  void setUserMeasurementProfileFunction(FuncPtrStringInt measurementProcessingFunction){
    MeasurementProcessingFunction=measurementProcessingFunction;
    measurement_function_is_set_= true;
  }

  void activate(){activated_=true;}
  void deactivate(){activated_=false;}

  bool isActivated(){return activated_;}



};
} // namespace pipert

#endif