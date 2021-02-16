#ifndef _MeasurmentBuffer_H_
#define _MeasurmentBuffer_H_

#include <thread>

#include "pipert/PacketBase.h"
#include "pipert/UDPConnection.h"

namespace pipert {

struct MeasurementType{


};

class MeasurmentBuffer {
 private:
  uint16_t BufferSize;
  uint16_t TimeIntervals;

 public:
  bool enableSending;

 private:
  pipert::UDPConnection connection;

  std::vector<std::string> measuremensListList;

  // this function will start sending the buffer via UDP connection each
  // timeInterval and clear measuremensListList
  void SendingJobCallBack() {

    while (enableSending) {
      if (measuremensListList.size() == BufferSize) {
        connection.send(measuremensListList);
        measuremensListList.clear();
      }
      std::this_thread::sleep_for(std::chrono::milliseconds(TimeIntervals));
    }
  }

  template <class T>
  std::string SerializeUserMeasurementProfile(pipert::MeasurementProfileBase* profileBase) {

    pipert::UserMeasurementProfile<T>* measurement_profile_ = (pipert::UserMeasurementProfile<T>*) (profileBase);
    string serializedMessage = "start,1,";
    serializedMessage.append(measurement_profile_->MesurementProfileName);
    serializedMessage.append(",");
    for(pipert::MeasurementEvent e : measurement_profile_->MeasurementsEventsLog){
      serializedMessage.append("EventStart");
      serializedMessage.append(",");
      serializedMessage.append(std::to_string(e.EventTime));
      serializedMessage.append(",");
      serializedMessage.append(std::to_string(e.ProcessStatus));
      serializedMessage.append(",");
      serializedMessage.append(std::to_string(*e.ThreadID));
      serializedMessage.append(",");
      serializedMessage.append(e.ChannelName);
      serializedMessage.append(",");
      serializedMessage.append("EventEnd");
      serializedMessage.append(",");

    }
    serializedMessage.append("ResultStart,");
    for(auto elem : measurement_profile_->getResult())
    {

      serializedMessage.append(elem.first );
      serializedMessage.append(",");
      serializedMessage.append(elem.second);
      serializedMessage.append(",");
    }
    serializedMessage.append("ResultStart,");
    serializedMessage.append("end");
    return serializedMessage;
  }

  string SerializeGeneralMeasurementProfile(
      pipert::MeasurementProfileBase* profileBase) {
    pipert::MeasurementProfile* measurement_profile_ = (pipert::MeasurementProfile*) (profileBase);
    string serializedMessage = "start,0,";
    serializedMessage.append(measurement_profile_->MesurementProfileName);
    serializedMessage.append(",");
    for(pipert::MeasurementEvent e : measurement_profile_->MeasurementsEventsLog){
      serializedMessage.append("EventStart");
      serializedMessage.append(",");
      serializedMessage.append(std::to_string(e.EventTime));
      serializedMessage.append(",");
      serializedMessage.append(std::to_string(e.ProcessStatus));
      serializedMessage.append(",");
      serializedMessage.append(std::to_string(*e.ThreadID));
      serializedMessage.append(",");
      serializedMessage.append(e.ChannelName);
      serializedMessage.append(",");
      serializedMessage.append("EventEnd");
      serializedMessage.append(",");

    }
    serializedMessage.append("end");
    return serializedMessage;
  }
 public:
  
  void generateBufferJob() {
    std::thread t = std::thread(&MeasurmentBuffer::SendingJobCallBack, this);
    t.join();


  }

  MeasurmentBuffer(UDPConnection udpConnection, uint16_t timeIntervals,
                   uint16_t bufferSize) {
    connection = udpConnection;
    BufferSize = bufferSize;
    TimeIntervals = timeIntervals;
    measuremensListList = {};
    enableSending = true;
  }

  MeasurmentBuffer(){}

  void pushMeasurement(MeasurementProfileBase* profile) {
    if (measuremensListList.size() < BufferSize) {
      measuremensListList.push_back(SerializeGeneralMeasurementProfile(profile));
    }
  }

  template <class T>
  void pushUserMeasurement(MeasurementProfileBase* profile) {
    if (measuremensListList.size() < BufferSize) {
      measuremensListList.push_back(SerializeUserMeasurementProfile<T>(profile));
    }
  }
};
}  // namespace pipert
#endif  //_PMeasurementProfileBase_H_
