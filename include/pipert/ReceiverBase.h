#ifndef _RECEIVERBASE_H_
#define _RECEIVERBASE_H_

#include <atomic>
#include <thread>

#include "pipert/UDPConnection.h"

namespace pipert {

class ReceiverBase {
 public:
  ReceiverBase(const ReceiverBase&) = delete;
  ReceiverBase& operator=(const ReceiverBase&) = delete;

  ReceiverBase(ReceiverBase&&);
  ReceiverBase& operator=(ReceiverBase&&);

  virtual void Start() = 0;
  virtual void Stop() = 0;

 protected:
  ReceiverBase(UDPConnection* connection)
    : connection_(connection) {}
  ~ReceiverBase() {}

  UDPConnection* connection_;
  std::thread polling_thread_;
  std::atomic_bool running_;

  virtual void Receive() = 0;
};

}  // namespace pipert

#endif  //_RECEIVERBASE_H_
