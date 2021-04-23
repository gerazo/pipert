#ifndef _PROTOCOL_H_
#define _PROTOCOL_H_

#include <cassert>
#include <cstddef>
#include <string.h>

#include "pipert/UDPConnection.h"

namespace pipert {

template <class T>
class Protocol {
 public:
  static const char kHandshakeSuccessMsg[];
  static const char kHandshakeErrorMsg[];

  Protocol(UDPConnection* connection);
  ~Protocol() = default;

  bool SenderSideHandshake();
  bool ReceiverSideHandshake();

 private:
  UDPConnection* connection_;
};

template <class T>
struct CompatibilityChecker {
  size_t sizeof_int, sizeof_intptr, sizeof_bool, sizeof_t,
         alignof_int, alignof_intptr, alignof_bool, alignof_t;

  void Init();
  bool operator==(const CompatibilityChecker<T>& other) const;
};

template <class T>
const char Protocol<T>::kHandshakeSuccessMsg[] = "CONN_EST";
template <class T>
const char Protocol<T>::kHandshakeErrorMsg[] = "CONN_ERR";

template <class T>
Protocol<T>::Protocol(UDPConnection* connection)
    : connection_(connection) {}

template <class T>
bool Protocol<T>::SenderSideHandshake() {
  CompatibilityChecker<T> local_properties;
  local_properties.Init();
  connection_->Send(&local_properties, sizeof(local_properties));
  char buffer[9];
  connection_->Receive(buffer, 9);
  if (!strcmp(buffer, kHandshakeErrorMsg)){
    return false;
  }
  return true;
}

template <class T>
bool Protocol<T>::ReceiverSideHandshake() {
  CompatibilityChecker<T> local_properties, remote_properties;
  local_properties.Init();
  connection_->Receive(&remote_properties, sizeof(remote_properties));
  if (local_properties == remote_properties) {
    connection_->Send(kHandshakeSuccessMsg, 9);
    return true;
  } else {
    connection_->Send(kHandshakeErrorMsg, 9);
    return false;
  }
}

template <class T>
void CompatibilityChecker<T>::Init() {
  sizeof_int = sizeof(int);
  sizeof_intptr = sizeof(int*);
  sizeof_bool = sizeof(bool);
  sizeof_t = sizeof(T);
  alignof_int = alignof(int);
  alignof_intptr = alignof(int*);
  alignof_bool = alignof(bool);
  alignof_t = alignof(T);
}

template <class T>
bool CompatibilityChecker<T>::operator==(const CompatibilityChecker<T>& other) const {
  return this->sizeof_int == other.sizeof_int &&
         this->sizeof_intptr == other.sizeof_intptr &&
         this->sizeof_bool == other.sizeof_bool &&
         this->sizeof_t == other.sizeof_t &&
         this->alignof_int == other.alignof_int &&
         this->alignof_intptr == other.alignof_intptr &&
         this->alignof_bool == other.alignof_bool &&
         this->alignof_t == other.alignof_t;
}

} // namespace pipert

#endif // _PROTOCOL_H_
