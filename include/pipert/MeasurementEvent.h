#ifndef _MeasurementEvent_H_
#define _MeasurementEvent_H_
#include "pipert/PacketBase.h"

namespace pipert {

class MeasurementEvent {
 public:
  const char* ChannelName;
  Timer::Time EventTime;
  MeasurementProfileProcessStatus ProcessStatus;
  int* ThreadID;

  string SerializeMeasurementEvent() {


    string serializedMessage = "";
    serializedMessage.append("EVENT,");
    serializedMessage.append(std::to_string(*ThreadID));
    serializedMessage.append(",");
    serializedMessage.append(ChannelName);
    serializedMessage.append(",");
    serializedMessage.append(std::to_string(ProcessStatus));
    serializedMessage.append(",");
    serializedMessage.append(std::to_string(EventTime));



    return serializedMessage;
  }
};


}// namespace pipert

#endif  //_MeasurementEvent