#ifndef _UDPCONNECTION_H_
#define _UDPCONNECTION_H_

#include <netinet/in.h>
#include <sys/socket.h>

namespace pipert {

/// This class is responsible for UDP communication.
///
/// This class enables the profiler to send the data through UDP protocol.
class UDPConnection {
 public:
  /// Constructs new UDP connection instance.
  ///
  /// \param port UDP connection port
  /// \param address UDP connection ip address
  UDPConnection(int port, const char* address);

  /// destructor of UDPConnection
  ~UDPConnection();

  /// Returns whether this UDP connection was properly initialized.
  bool IsConnected() const { return socket_filedesc_ != -1; }

  /// Sends the data through UDP connection.
  /// \param buffer holds the buffer that needs to be sent.
  /// \param size holds the buffer size.
  void Send(void* buffer, int size);

 private:
  int socket_filedesc_;
  struct sockaddr_in server_address_;
};

}  // namespace pipert

#endif  // end of _UDPCONNECTION_H
