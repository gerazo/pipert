#include "gtest/gtest.h"
#include "pipert/UDPConnection.h"
#include "pipert/MeasurementProfileBase.h"


namespace {
class Human {
 public:
  std::string name_;
  int age_;

  Human(const std::string& name, int age) : name_(name), age_(age) {}
  Human() {}
};

TEST(UDPConnection, HandlingOfRValues) {
pipert::UDPConnection connection=pipert::UDPConnection();
std::vector<pipert::MeasurementProfileBase*> measurements;
pipert::MeasurementProfileBase* mesurement1 = pipert::MeasurementProfileBase(true);
measurements.push_back(mesurement1);
connection.send<Human>(measurements);

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
//   EXPECT_THROW()
// }