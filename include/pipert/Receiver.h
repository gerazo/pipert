#ifndef _RECEIVER_H_
#define _RECEIVER_H_

#include "pipert/Channel.h"
#include "pipert/NetworkPacket.h"
#include "pipert/Protocol.h"
#include "pipert/ReceiverBase.h"

namespace pipert {

template <class T>
class Receiver : public ReceiverBase {
 public:
  Receiver(Scheduler* scheduler, UDPConnection* connection,
           Channel<T>* ch_to_write);

  Receiver(Receiver&&) = default;
  Receiver& operator=(Receiver&&) = default;

  void Start() override;
  void Stop() override;

 private:
  Channel<T>* ch_to_write_;

  void Receive() override;
};

template <class T>
Receiver<T>::Receiver(Scheduler* scheduler,
                      UDPConnection* connection,
                      Channel<T>* ch_to_write)
    : ReceiverBase(connection), ch_to_write_(ch_to_write) {
  Protocol<T> protocol(connection);
  if(protocol.ReceiverSideHandshake())
    scheduler->AddReceiver(this);
  else
    scheduler->SetStateInvalid();
}

template <class T>
void Receiver<T>::Start() {
  assert(!running_.load(std::memory_order_acquire));
  if (running_.load(std::memory_order_acquire))
    return;
  running_.store(true, std::memory_order_release);
  polling_thread_ = std::thread(&Receiver<T>::Receive, this);
}

template <class T>
void Receiver<T>::Stop() {
  assert(running_.load(std::memory_order_acquire));
  if (!running_.load(std::memory_order_acquire))
    return;
  running_.store(false, std::memory_order_release);
  if (polling_thread_.joinable())
    polling_thread_.join();
}

template <class T>
void Receiver<T>::Receive() {
  while(running_.load(std::memory_order_acquire)) {
    pipert::NetworkPacket<T> network_packet;
    if (connection_->Poll() > 0) {
      connection_->Receive(&network_packet, sizeof(network_packet));
      ch_to_write_->Acquire(network_packet.timestamp(), network_packet.data());
    }
  }
}

}  // namespace pipert

#endif  //_RECEIVER_H_
