#include "gtest/gtest.h"
#include "pipert/UDPConnection.h"
#include "pipert/MeasurementProfileBase.h"
#include "pipert/MeasurementEvent.h"



namespace {

TEST(EventTest, HandlingOfRValues) {
pipert::MeasurementEvent event;
event.ChannelName ="channal1";
event.EventTime = 1603716857718209;
event.ProcessStatus =pipert::MeasurementProfileProcessStatus::ExecuteStart;
int threadID=15;
int* threadidp=&threadID;
event.ThreadID=threadidp;

string serializedMessage= event.SerializeMeasurementEvent();

  EXPECT_GE(event.ChannelName,"channal1");
  EXPECT_GE(serializedMessage,"EVENT,15,channal1,4,1603716857718209");
}
}

// TEST(PacketTest, WrongInputShouldThrowAnError){
//   // Given
//   auto now = pipert::Timer::time();
//   // When
//   // Then
//   EXPECT_THROW()
// }