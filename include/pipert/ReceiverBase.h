#ifndef _RECEIVERBASE_H_
#define _RECEIVERBASE_H_

#include <atomic>
#include <thread>

#include "pipert/UDPConnection.h"

namespace pipert {

/// Abstract base class for the Receiver.
///
/// This lightweight non-templated class helps handle Receiver objects
/// in the other parts of the implementation. Pure virtual functions are
/// overridden in the Receiver subclass.
class ReceiverBase {
 public:
  ReceiverBase(const ReceiverBase&) = delete;
  ReceiverBase& operator=(const ReceiverBase&) = delete;

  ReceiverBase(ReceiverBase&&);
  ReceiverBase& operator=(ReceiverBase&&);

  virtual void Start() = 0;
  virtual void Stop() = 0;

 protected:
  /// Constructor of ReceiverBase.
  /// \param connection Network properties of the sender side.
  ReceiverBase(UDPConnection* connection)
    : connection_(connection) {}
  ~ReceiverBase() {}

  UDPConnection* connection_;  ///< Connection details of the sender computer.
  std::thread polling_thread_; ///< Dedicated thread for Receive() data.
  std::atomic_bool running_;   ///< Switches the states in the Receive method.

  virtual void Receive() = 0;
};

}  // namespace pipert

#endif  //_RECEIVERBASE_H_
