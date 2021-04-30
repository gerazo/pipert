#include "pipert/UDPConnection.h"

#include <cassert>
#include <cstdint>
#include <cstring>
#include <string>

#include <arpa/inet.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>

namespace pipert {
UDPConnection::UDPConnection(int remote_port,
                             const char* remote_address,
                             int max_packet_size,
                             int polling_timeout) {
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
  max_packet_size_ = max_packet_size;
  polling_timeout_ = polling_timeout;
  int val = IP_PMTUDISC_DO;
  setsockopt(socket_filedesc_, IPPROTO_IP, IP_MTU_DISCOVER, &val, sizeof(val));
  fd_.fd = socket_filedesc_;
  assert(socket_filedesc_ != -1);  // opening socket failed
}

UDPConnection::UDPConnection(int binding_port,
                             int max_packet_size,
                             int polling_timeout) {
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
  max_packet_size_ = max_packet_size;
  polling_timeout_ = polling_timeout;
  int val = IP_PMTUDISC_DO;
  setsockopt(socket_filedesc_, IPPROTO_IP, IP_MTU_DISCOVER, &val, sizeof(val));
  fd_.fd = socket_filedesc_;
  assert(socket_filedesc_ != -1);  // opening socket failed
}

UDPConnection::~UDPConnection() {
  if (IsConnected()) {
    close(socket_filedesc_);
  }
}

int UDPConnection::SetBlockingMode(bool is_blocking) {
  const int flags = fcntl(socket_filedesc_, F_GETFL, 0);
  if (((flags & O_NONBLOCK) && !is_blocking) ||
      (!(flags & O_NONBLOCK) && is_blocking))
    return 0;
  return fcntl(socket_filedesc_, F_SETFL, is_blocking ? flags ^ O_NONBLOCK : flags | O_NONBLOCK);
}

void UDPConnection::DetectMaxPacketSize() {
  max_packet_size_ = 512;
  char cmd[128];
  char cmd_prefix[64] = "ping ";
  std::strcat(cmd_prefix, inet_ntoa(remote_address_.sin_addr));
  std::strcat(cmd_prefix, " -c3 -w3 -M do -s ");
  char cmd_postfix[32];
  snprintf(cmd_postfix, 32, "%d%s", max_packet_size_, " > /dev/null 2>&1");
  std::strcpy(cmd, cmd_prefix);
  std::strcat(cmd, cmd_postfix);
  while(!system(cmd)) {
    max_packet_size_ *= 2;
    snprintf(cmd_postfix, 32, "%d%s", max_packet_size_, " > /dev/null 2>&1");
    std::strcpy(cmd, cmd_prefix);
    std::strcat(cmd, cmd_postfix);
  }
}

void UDPConnection::Send(const void* buffer, int size) {
  assert(size > 0 && size <= max_packet_size_);
  if (IsConnected()) {
    sendto(socket_filedesc_, buffer, size, 0,
           (struct sockaddr*)&remote_address_, sizeof(remote_address_));
  }
}

int UDPConnection::Poll() {
  if (IsConnected()) {
    fd_.events = POLLIN;
    return poll(&fd_, 1, polling_timeout_);
  }
  return -1;
}

int UDPConnection::Receive(void* buffer, int size) {
  assert(size > 0 && size <= max_packet_size_);
  if (IsConnected()) {
    socklen_t len = sizeof(remote_address_);
    return recvfrom(socket_filedesc_, buffer, size, 0,
                    (struct sockaddr *)&remote_address_, &len);
  }
  return -1;
}

}  // namespace pipert
