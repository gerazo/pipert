#ifndef _PROTOCOL_H_
#define _PROTOCOL_H_

#include <cassert>
#include <cstddef>
#include <string.h>

#include "pipert/UDPConnection.h"

namespace pipert {

/// Network protocol to establish connection between computers.
///
/// Protocol is created at the construction of the sender side
/// channel (see Scheduler::CreateSenderChannel()) and the
/// receiver side object (see Receiver::Receiver()). It is used
/// for checking the basic porperties of the communication
/// participants. For these checks the protocol uses a helper class
/// named CompatibilityChecker and the handshake processes.
///
/// \tparam T Type of user data which is contained by the Packet.
template <class T>
class Protocol {
 public:
  /// This message being sent after a successful handshake.
  static const char kHandshakeSuccessMsg[];
  /// This message being sent after a failed handshake.
  static const char kHandshakeErrorMsg[];

  /// Constructs a new Protocol object to call the proper
  /// handshake function.
  /// \param connection Network properties of the other participant.
  Protocol(UDPConnection* connection);
  ~Protocol() = default;

  /// Sender side handshake function which sends the
  /// collected data of the local properties and waits
  /// for the answer about the success of the handshake.
  /// \return Success of the handshake.
  bool SenderSideHandshake();

  /// Receiver side handshake function which
  /// waits for the properties of the sender side,
  /// compares them with the local properties and
  /// sends back the proper result message.
  /// \return Success of the handshake.
  bool ReceiverSideHandshake();

 private:
  UDPConnection* connection_;  ///< Connection for the handshake functions.
};

/// Simple helper struct to determine the possible
/// differences between the sender and receiver computers.
/// \tparam T Type of user data which is contained by the Packet.
template <class T>
struct CompatibilityChecker {
  /// Basic properties to be checked in the handshakes.
  size_t sizeof_int, sizeof_intptr, sizeof_bool, sizeof_t,
         alignof_int, alignof_intptr, alignof_bool, alignof_t;

  /// Fills the given variables with the local computer properties.
  void Init();
  /// Overloaded equality operator to compare the member variables.
  /// \return Whether all of the member variables are equal.
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
