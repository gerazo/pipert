#ifndef _RECEIVER_H_
#define _RECEIVER_H_

#include "pipert/Channel.h"
#include "pipert/NetworkPacket.h"
#include "pipert/Protocol.h"
#include "pipert/ReceiverBase.h"

namespace pipert {


/// Receiver side object of the pipeline over the network.
///
/// Receiver allocates it's own thread where it keeps polling
/// and reading data from the given socket. Socket handling functions
/// come from the constructor parameter named connection. See
/// UDPConnection class for details.
///
/// Receiver is handled automatically from the Scheduler
/// (see SchedulerImpl::Start() and SchedulerImpl::Stop()).
/// To use a Receiver in your pipeline you just need to create
/// the object with the proper parameters.
///
/// \tparam T Type of the Channel where the received data goes.
template <class T>
class Receiver : public ReceiverBase {
 public:
  /// Parametrized constructor of Receiver.
  /// Doing the handshake and register Receiver into the
  /// Scheduler is done during the construction of this object.
  /// \param scheduler Main Scheduler for setting status after the handshake
  ///                  and registering created receiver.
  /// \param connection Network properties of the sender side.
  /// \param ch_to_write Channel to write the received Packets.
  Receiver(Scheduler* scheduler, UDPConnection* connection,
           Channel<T>* ch_to_write);

  Receiver(Receiver&&) = default;
  Receiver& operator=(Receiver&&) = default;

  /// Starts the receiver thread.
  void Start() override;
  /// Stops the receiver thread.
  void Stop() override;

 private:
  Channel<T>* ch_to_write_;  ///< Channel where the received Packets are written.

  /// Reads from the network socket and puts data to
  /// the given Channel.
  void ReceivePacket() override;
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
  receiver_thread_ = std::thread(&Receiver<T>::ReceivePacket, this);
}

template <class T>
void Receiver<T>::Stop() {
  assert(running_.load(std::memory_order_acquire));
  if (!running_.load(std::memory_order_acquire))
    return;
  running_.store(false, std::memory_order_release);
  if (receiver_thread_.joinable())
    receiver_thread_.join();
}

template <class T>
void Receiver<T>::ReceivePacket() {
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
