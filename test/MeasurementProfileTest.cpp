#include "gtest/gtest.h"
#include "pipert/UDPConnection.h"
#include "pipert/MeasurementProfileBase.h"
#include "pipert/MeasurementProfile.h"
#include "pipert/MeasurementEvent.h"


namespace {

TEST(MeasurementProfile, HandlingOfRValues) {
  pipert::MeasurementProfile profile;
  profile.MesurementProfileName ="M1";

  pipert::MeasurementEvent measurement_event;
  measurement_event.EventTime=pipert::Timer::time();
   measurement_event.ProcessStatus=pipert::MeasurementProfileProcessStatus::ExecuteStart;
   measurement_event.ChannelName="C1";
  profile.MeasurementsEventsLog.push_back(measurement_event);

  EXPECT_GE(profile.MesurementProfileName,"M1");
  EXPECT_GE(profile.MeasurementsEventsLog.at(0).ChannelName,"C1");


}
}

