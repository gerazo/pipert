#ifndef _PROTOCOLHANDSHAKE_H_
#define _PROTOCOLHANDSHAKE_H_

#include "pipert/UDPConnection.h"

#include <cstddef>

namespace pipert {

template <class T>
class Protocol {
 public:
  static char HandshakeSuccessMsg[];
  static char HandshakeErrorMsg[];

  Protocol(const UDPConnection& connection);
  ~Protocol() = default;

  void SenderSideHandshake();
  void ReceiverSideHandshake();

 private:
  UDPConnection connection_;
};

template <class T>
struct CompatibilityChecker {
  size_t sizeof_int, sizeof_intptr, sizeof_bool, sizeof_t,
         alignof_int, alignof_intptr, alignof_bool, alignof_t;

  void Init();
  bool operator==(const CompatibilityChecker<T>& other) const;
};

} // namespace pipert

#endif // _PROTOCOLHANDSHAKE_H_
