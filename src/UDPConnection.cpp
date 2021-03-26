#include "pipert/UDPConnection.h"

#include <cassert>
#include <cstdint>
#include <cstring>

#include <arpa/inet.h>
#include <sys/types.h>
#include <unistd.h>

namespace pipert {
UDPConnection::UDPConnection(int remote_port, const char* remote_address) {
  socket_filedesc_ = socket(AF_INET, SOCK_DGRAM, 0);
  if (socket_filedesc_ != -1) {
    memset(&remote_address_, 0, sizeof(remote_address_));
    /// Filling server information
    remote_address_.sin_family = AF_INET;
    remote_address_.sin_port = htons((std::uint16_t)remote_port);
    int ok = inet_aton(remote_address, &remote_address_.sin_addr);
    if (!ok) {
      assert(ok == 1);  // getting address failed
      socket_filedesc_ = -1;
      return;
    }
  }
  assert(socket_filedesc_ != -1);  // opening socket failed
}

UDPConnection::UDPConnection(int binding_port) {
  socket_filedesc_ = socket(AF_INET, SOCK_DGRAM, 0);
  if (socket_filedesc_ != -1) {
    memset(&remote_address_, 0, sizeof(remote_address_));
    memset(&local_address_, 0, sizeof(local_address_));
    local_address_.sin_family = AF_INET;
    local_address_.sin_addr.s_addr = INADDR_ANY;
	  local_address_.sin_port = htons((std::uint16_t)binding_port);
    /// Bind the socket with the local address
    int ok = bind(socket_filedesc_, (const struct sockaddr *)&local_address_,
			        sizeof(local_address_));
    if (ok) {
      assert(ok == 1);  // binding failed
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
           (struct sockaddr*)&remote_address_, sizeof(remote_address_));
  }
}

int UDPConnection::Receive(void* buffer, int size) {
  assert(size > 0 && size <= 508);
  if (IsConnected()) {
    socklen_t len = sizeof(remote_address_);
    return recvfrom(socket_filedesc_, buffer, size, 0,
                    (struct sockaddr *)&remote_address_, &len);
  }
  return -1;
}

}  // namespace pipert
