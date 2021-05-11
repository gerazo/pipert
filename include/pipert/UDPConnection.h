#ifndef _UDPCONNECTION_H_
#define _UDPCONNECTION_H_

#include <netinet/in.h>
#include <sys/socket.h>
#include <poll.h>

namespace pipert {

/// This class is responsible for UDP communication.
class UDPConnection {
 public:
  /// Constructs new UDP connection instance.
  ///
  /// \param remote_port UDP connection port
  /// \param remote_address UDP connection ip address
  /// \param max_packet_size Allowed maximum packet size in bytes (default: 508)
  /// \param polling_timeout waiting time in milliseconds for an event to
  ///                        occur on the file descriptor. (Default: 1000)
  UDPConnection(int remote_port, const char* remote_address,
                int max_packet_size = 508, int polling_timeout = 1000);

  /// Constructs new UDP connection instance.
  ///
  /// \param binding_port UDP connection port
  /// \param max_packet_size Allowed maximum packet size in bytes (default: 508)
  /// \param polling_timeout waiting time in milliseconds for an event to
  ///                        occur on the file descriptor. (Default: 1000)
  UDPConnection(int binding_port, int max_packet_size = 508,
                int polling_timeout = 1000);

  UDPConnection(const UDPConnection&) = delete;
  UDPConnection& operator=(const UDPConnection&) = delete;

  /// Destructor of UDPConnection.
  ~UDPConnection();

  /// Returns whether the UDP connection was properly initialized.
  bool IsConnected() const { return socket_filedesc_ != -1; }

  /// Sets socket's blocking mode
  int SetBlockingMode(bool is_blocking);

  /// Tests network to detect the biggest possible packet size for
  /// sending data without fragmentation. This function may require up
  /// to 20 seconds (it depends on the biggest packet size) to complete.
  void DetectMaxPacketSize();

  /// Sends the data through UDP connection.
  /// \param buffer holds the buffer that needs to be sent.
  /// \param size holds the buffer size.
  void Send(const void* buffer, int size);

  /// Waits for some event on the file descriptor.
  /// \return A positive value indicates the total number of file
  ///         descriptors that have been selected. A value of 0
  ///         indicates that the call timed out. Upon failure, return
  ///         value is -1.
  int Poll();

  /// Receives data from UDP connection.
  /// \param buffer provides the buffer in which to receive data.
  /// \param size holds the buffer size.
  /// \return The number of bytes received, or -1 if an error occurred.
  int Receive(void* buffer, int size);

 private:
  int socket_filedesc_;
  int max_packet_size_;
  int polling_timeout_;
  struct sockaddr_in local_address_, remote_address_;
  struct pollfd fd_;
};

}  // namespace pipert

#endif  // end of _UDPCONNECTION_H
