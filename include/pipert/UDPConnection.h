#ifndef _UDPCONNECTION_H_
#define _UDPCONNECTION_H_

#include <netinet/in.h>
#include <sys/socket.h>

namespace pipert {

/// This class is responsible for UDP communication.
class UDPConnection {
 public:
  /// Constructs new UDP connection instance.
  ///
  /// \param remote_port UDP connection port
  /// \param remote_address UDP connection ip address
  UDPConnection(int remote_port, const char* remote_address);

  /// Constructs new UDP connection instance.
  ///
  /// \param binding_port UDP connection port
  UDPConnection(int binding_port);

  UDPConnection(const UDPConnection&) = delete;
  UDPConnection& operator=(const UDPConnection&) = delete;

  /// Destructor of UDPConnection.
  ~UDPConnection();

  /// Returns whether the UDP connection was properly initialized.
  bool IsConnected() const { return socket_filedesc_ != -1; }

  /// Sets socket's blocking mode
  int SetBlockingMode(bool is_blocking);

  /// Sends the data through UDP connection.
  /// \param buffer holds the buffer that needs to be sent.
  /// \param size holds the buffer size.
  void Send(void* buffer, int size);

  /// Receives data from UDP connectom.
  /// \param buffer provides the buffer in which to receive data.
  /// \param size holds the buffer size.
  /// \return The number of bytes received, or -1 if an error occurred.
  int Receive(void* buffer, int size);

 private:
  int socket_filedesc_;
  struct sockaddr_in local_address_, remote_address_;
};

}  // namespace pipert

#endif  // end of _UDPCONNECTION_H
