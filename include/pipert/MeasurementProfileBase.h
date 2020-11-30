#ifndef _MeasurementProfileBase_H_
#define _MeasurementProfileBase_H_

namespace pipert {

// this enum define the last level of processing the packet reached in the
// channel
enum MeasurementProfileProcessStatus {
  NoStatus,
  Acquired,
  Pushed,
  Popped,
  Executed,
  Dropped
};
class MeasurementProfileBase {
 public:
  bool IsOptionalUserProfile;
  MeasurementProfileProcessStatus processState;

  MeasurementProfileBase(bool isOptionalUserProfile) {
    IsOptionalUserProfile = isOptionalUserProfile;
  }

  // must
};
}  // namespace pipert
#endif  //_PMeasurementProfileBase_H_
