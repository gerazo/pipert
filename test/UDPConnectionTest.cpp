#include "gtest/gtest.h"

#include <cstdint>
#include <arpa/inet.h>
#include <sys/types.h>
#include <unistd.h>

#include "UDPConnection.h"

namespace {

const std::uint16_t server_port = 8888;
const int buffer_size = 508;
std::uint8_t buffer[buffer_size];
int sockfd;

void PrepareToReceive() {
  struct sockaddr_in servaddr;

  // Creating socket file descriptor
  if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
    ASSERT_TRUE(false);  // socket creation failed
  }

  memset(&servaddr, 0, sizeof(servaddr));

  // Filling server information
  servaddr.sin_family = AF_INET;  // IPv4
  servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
  servaddr.sin_port = htons(server_port);

  // Bind the socket with the server address
  if (bind(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
    ASSERT_TRUE(false);  // bind failed
  }
}

bool Receive() {
  struct sockaddr_in cliaddr;
  memset(&cliaddr, 0, sizeof(cliaddr));
  socklen_t len = sizeof(cliaddr);
  int retbuflen = recvfrom(sockfd, buffer, buffer_size, 0,
                           (struct sockaddr *)&cliaddr, &len);
  if (retbuflen != buffer_size) return false;
  for (int i = 0; i < buffer_size; i++) {
    if (buffer[i] != 7) return false;
  }
  close(sockfd);
  return true;
}

}  // namespace

TEST(UDPConnectionTest, TestSending) {
  PrepareToReceive();
  for (int i = 0; i < buffer_size; i++) {
    buffer[i] = 7;
  }
  char ip[] = "127.0.0.1";
  pipert::UDPConnection connection = pipert::UDPConnection(server_port, ip);
  connection.Send(buffer, buffer_size);
  EXPECT_TRUE(connection.IsConnected());
  for (int i = 0; i < buffer_size; i++) {
    buffer[i] = 0;
  }
  EXPECT_TRUE(Receive());
}

