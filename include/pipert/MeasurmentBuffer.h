#ifndef _MeasurmentBuffer_H_
#define _MeasurmentBuffer_H_

#include <thread>

#include "pipert/PacketBase.h"

namespace pipert {

class MeasurmentBuffer {
 private:
  uint16_t BufferSize;
  uint16_t TimeIntervals;

 public:
  bool enableSending;

 private:
  UDPConnection connection;

  std::vector<MeasurementProfileBase> measuremensListList;

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

 public:
  void generateBufferJob() {
    std::thread t = std::thread(&MeasurmentBuffer::SendingJobCallBack, this);
    t.detach();
  }

  MeasurmentBuffer(UDPConnection udpConnection, uint16_t timeIntervals,
                   uint16_t bufferSize) {
    connection = udpConnection;
    BufferSize = bufferSize;
    TimeIntervals = timeIntervals;
    measuremensListList = {};
    enableSending = true;
  }

  void pushMeasurement(MeasurementProfileBase profile) {
    if (measuremensListList.size() < BufferSize) {
      measuremensListList.push_back(profile);
    }
  }
};
}  // namespace pipert
#endif  //_PMeasurementProfileBase_H_
