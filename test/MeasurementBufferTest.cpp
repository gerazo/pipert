#include "gtest/gtest.h"
#include "pipert/UDPConnection.h"
#include "pipert/MeasurementProfileBase.h"
#include "pipert/MeasurmentBuffer.h"
#include "pipert/MeasurementProfile.h"
#include "pipert/MeasurementEvent.h"
#include "string.h"
#define UNUSED(x) (void)(x)


namespace {

TEST(UDPConnection, HandlingOfRValues) {
  pipert::UDPConnection connection=pipert::UDPConnection();
  pipert::MeasurementEvent event;
  event.ChannelName ="channal1";
  event.EventTime = 1603716857718209;
  event.ProcessStatus =pipert::MeasurementProfileProcessStatus::ExecuteStart;
  int threadID=15;
  int* threadidp=&threadID;
  event.ThreadID=threadidp;



  pipert::MeasurementProfile mesurement1_p_;

  mesurement1_p_.MeasurementsEventsLog.push_back(event);


  mesurement1_p_.MesurementProfileName="Profile1";
  pipert::MeasurementProfileBase* mesurement1_p_base_= dynamic_cast<pipert::MeasurementProfileBase*> (&mesurement1_p_);


  pipert::MeasurmentBuffer buffer(connection,1000,3);
  buffer.pushMeasurement(mesurement1_p_base_);
  buffer.pushMeasurement(mesurement1_p_base_);
  buffer.pushMeasurement(mesurement1_p_base_);


  buffer.generateBufferJob();
  int i=0;
  int j=i+1;
  EXPECT_EQ(j,1);

}
}

// TEST(PacketTest, WrongInputShouldThrowAnError){
//   // Given
//   auto now = pipert::Timer::time();
//   // When
//   // Then

/*pipert::MeasurementProfileBase &p2=mesurement1_p_Base_;
pipert::MeasurementProfile&  MB=dynamic_cast<pipert::MeasurementProfile&>( p2);
MB.MesurementProfileName="Name";*/

//   EXPECT_THROW()
// }