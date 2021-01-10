#include "gtest/gtest.h"
#include "pipert/UDPConnection.h"
#include "pipert/MeasurementProfileBase.h"
#include "pipert/MeasurmentBuffer.h"
#include "pipert/MeasurementProfile.h"
#include "pipert/UserMeasurementProfile.h"

#include "string.h"
#define UNUSED(x) (void)(x)


namespace pipert{

class Human {
 public:
  std::string name_;
  int age_;

  Human(const std::string& name, int age) : name_(name), age_(age) {}
  Human() {}
};
std::map<std::string, std::string> getHumanNameAndAge(Human h){
  std::map<std::string, std::string> result;

  // Inserting data in std::map
  bool age_larger_than_30_ = h.age_>30;
  string human_name_and_age = h.name_ + " "+std::to_string(h.age_);
  result.insert(std::make_pair("isAgeLargerThan30", std::to_string(age_larger_than_30_)));
  result.insert(std::make_pair("name_age", human_name_and_age));


return result;
}

TEST(userProfileTest, HandlingOfRValues) {


//  pipert::UDPConnection connection=pipert::UDPConnection();
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
 pipert::UserMeasurementProfile<Human> UMP(h,getHumanNameAndAge);
  UMP.MeasurementsEventsLog.push_back(event);

  std::map<std::string, std::string> userCallBackFunctionResult = UMP.getResult();

  EXPECT_EQ(userCallBackFunctionResult["isAgeLargerThan30"],"0");
  EXPECT_EQ(userCallBackFunctionResult["name_age"],"Jimi Hendrix 28");

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