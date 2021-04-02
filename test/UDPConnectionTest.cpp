#include "gtest/gtest.h"

#include <cstdint>
#include <arpa/inet.h>
#include <sys/types.h>
#include <unistd.h>

#include "pipert/UDPConnection.h"

namespace {

const std::uint16_t server_port = 8888;
const int buffer_size = 508;
std::uint8_t buffer[buffer_size];

}  // namespace

TEST(UDPConnectionTest, TestSendingReceiving) {
  pipert::UDPConnection connection_receiver(server_port);
  for (int i = 0; i < buffer_size; i++) {
    buffer[i] = 7;
  }
  char ip[] = "127.0.0.1";
  pipert::UDPConnection connection(server_port, ip);
  connection.Send(buffer, buffer_size);
  EXPECT_TRUE(connection.IsConnected());
  for (int i = 0; i < buffer_size; i++) {
    buffer[i] = 0;
  }
  
  int retbuflen = connection_receiver.Receive(buffer, buffer_size);
  EXPECT_EQ(retbuflen, buffer_size);
  for (int i = 0; i < buffer_size; i++) {
    EXPECT_EQ(buffer[i], 7);
  }
}
