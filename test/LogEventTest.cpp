#include "gtest/gtest.h"
#include "pipert/LogEvent.h"

namespace {

const char kEventType1[] = "Mass Turbulence";
const char kEventType2[] = "Psychokinetic Activity";
const char kEventType3[] = "Unused Event Type";

}  // namespace

TEST(LogEventTest, EventCreationIsOK) {
  pipert::LogEvent<kEventType1> le(1.0);
  EXPECT_EQ(le.GetValue(), 1.0);
  EXPECT_EQ(le.GetName(), kEventType1);
}

TEST(LogEventTest, MultipleEventCreationOfSameTypeIsOK) {
  pipert::LogEvent<kEventType1> le1(1.0);
  pipert::LogEvent<kEventType1> le2(2.0);
  EXPECT_EQ(le1.GetValue(), 1.0);
  EXPECT_EQ(le1.GetName(), kEventType1);
  EXPECT_EQ(le2.GetValue(), 2.0);
  EXPECT_EQ(le2.GetName(), kEventType1);
}

TEST(LogEventTest, DifferentEventCreationIsOK) {
  pipert::LogEvent<kEventType1> le1(1.0);
  pipert::LogEvent<kEventType2> le2(2.0);
  EXPECT_EQ(le1.GetValue(), 1.0);
  EXPECT_EQ(le1.GetName(), kEventType1);
  EXPECT_EQ(le2.GetValue(), 2.0);
  EXPECT_EQ(le2.GetName(), kEventType2);
}

bool RegistrarHasEventTypeID(const char* type_id) {
  for (int i = 0; i < pipert::LogEventTypeRegistrar::GetRegisterLength(); i++) {
    if (type_id == pipert::LogEventTypeRegistrar::GetLogEventTypeID(i))
      return true;
  }
  return false;
}

TEST(LogEventTest, RegistrarHasAllInstantiatedTypes) {
  EXPECT_GE(pipert::LogEventTypeRegistrar::GetRegisterLength(), 2);
  EXPECT_TRUE(RegistrarHasEventTypeID(kEventType1));
  EXPECT_TRUE(RegistrarHasEventTypeID(kEventType2));
  EXPECT_FALSE(RegistrarHasEventTypeID(kEventType3));
}
