#include "gtest/gtest.h"
#include "pipert/UDPConnection.h"
#include "pipert/MeasurementProfileBase.h"
#include "pipert/MeasurmentBuffer.h"


namespace {

TEST(UDPConnection, HandlingOfRValues) {
pipert::UDPConnection connection=pipert::UDPConnection();
std::vector<pipert::MeasurementProfileBase> measurements;
pipert::MeasurementProfileBase mesurement1 = pipert::MeasurementProfileBase(true);
measurements.push_back(mesurement1);
measurements.push_back(mesurement1);
measurements.push_back(mesurement1);


pipert::MeasurmentBuffer buffer(connection,1000,3);
buffer.pushMeasurement(mesurement1);
buffer.pushMeasurement(mesurement1);
buffer.pushMeasurement(mesurement1);
buffer.pushMeasurement(mesurement1);
buffer.generateBufferJob();
buffer.enableSending = false;
buffer.pushMeasurement(mesurement1);
buffer.pushMeasurement(mesurement1);
buffer.pushMeasurement(mesurement1);
buffer.generateBufferJob();
buffer.enableSending = false;

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