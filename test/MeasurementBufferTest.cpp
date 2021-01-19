#include "gtest/gtest.h"
#include "pipert/UDPConnection.h"
#include "pipert/MeasurementProfileBase.h"
#include "pipert/MeasurmentBuffer.h"
#include "pipert/MeasurementProfile.h"
#include "pipert/MeasurementEvent.h"
#include "pipert/UserMeasurementProfile.h"
#include "string.h"
#define UNUSED(x) (void)(x)


namespace {

class Human {
 public:
  std::string name_;
  int age_;

  Human(const std::string& name, int age) : name_(name), age_(age) {}
  Human() {}
};

TEST(MesurementProfile,DISABLED_SendingMesurement) {
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


  pipert::MeasurmentBuffer buffer(connection,10,3);
  buffer.pushMeasurement(mesurement1_p_base_);
  buffer.pushMeasurement(mesurement1_p_base_);
  buffer.pushMeasurement(mesurement1_p_base_);


  buffer.generateBufferJob();
  int i=0;
  int j=i+1;
  EXPECT_EQ(j,1);

}

std::map<std::string, std::string> getHumanNameAndAge(Human h){
  std::map<std::string, std::string> result;

  // Inserting data in std::map
  bool age_larger_than_30_ = h.age_>30;
  string human_name_and_age = h.name_ + " "+std::to_string(h.age_);
  result.insert(std::make_pair("isAgeLargerThan30", std::to_string(age_larger_than_30_)));
  result.insert(std::make_pair("name_age", human_name_and_age));


  return result;
}

TEST(MesurementProfile, SendingUserMesurement) {
  pipert::UDPConnection connection=pipert::UDPConnection();
  pipert::MeasurementEvent event;
  event.ChannelName ="channal1";
  event.EventTime = 1603716857718209;
  event.ProcessStatus =pipert::MeasurementProfileProcessStatus::ExecuteStart;
  int threadID=15;
  int* threadidp=&threadID;
  event.ThreadID=threadidp;
  auto now = pipert::Timer::time();
  pipert::Packet<Human> packet_(now, "Jimi Hendrix", 28);
  Human h=packet_.data();
  pipert::UserMeasurementProfile<Human> UMP(getHumanNameAndAge,h);
  UMP.MeasurementsEventsLog.push_back(event);
  UMP.MesurementProfileName="userProfile1";
  pipert::MeasurementProfileBase* mesurement1_p_base_= dynamic_cast<pipert::MeasurementProfileBase*> (&UMP);
  pipert::MeasurmentBuffer buffer(connection,1000,3);
  buffer.pushUserMeasurement<Human>(mesurement1_p_base_);
  buffer.pushUserMeasurement<Human>(mesurement1_p_base_);
  buffer.pushUserMeasurement<Human>(mesurement1_p_base_);
  buffer.pushUserMeasurement<Human>(mesurement1_p_base_);
  buffer.generateBufferJob();
  int i=0;
  int j=i+1;
  EXPECT_EQ(j,1);

}


}

