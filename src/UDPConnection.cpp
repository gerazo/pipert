#include "UDPConnection.h"


#include <cassert>
#include <cstdint>
#include <cstring>


#include <arpa/inet.h>
#include <sys/types.h>
#include <unistd.h>


namespace pipert {

UDPConnection::UDPConnection(int port, const char* address) {
  socket_filedesc_ = socket(AF_INET, SOCK_DGRAM, 0);
  if (socket_filedesc_ != -1) {
    memset(&server_address_, 0, sizeof(server_address_));
    /// Filling server information
    server_address_.sin_family = AF_INET;
    server_address_.sin_port = htons((std::uint16_t)port);
    int ok = inet_aton(address, &server_address_.sin_addr);
    if (!ok) {
      assert(ok == 1);  // getting address failed
      socket_filedesc_ = -1;
      return;
    }
  }
  assert(socket_filedesc_ != -1);  // opening socket failed
}

UDPConnection::~UDPConnection() {
  if (IsConnected()) {
    close(socket_filedesc_);
  }
}

void UDPConnection::Send(void* buffer, int size) {
  assert(size > 0 && size <= 508);
  if (IsConnected()) {
    sendto(socket_filedesc_, buffer, size, 0,
           (struct sockaddr*)&server_address_, sizeof(server_address_));
  }
}

}  // namespace pipert

