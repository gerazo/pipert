#include "pipert/Profiler.h"

#include <cassert>
#include <cstdlib>
#include <cstring>
#include <utility>

#include "ProfilerImpl.h"

namespace pipert {

const char Profiler::kNoDestination[] = "";
const char Profiler::kDestinationUDPScheme[] = "udp:";
const char Profiler::kDestinationFileScheme[] = "file:";

Profiler::Profiler(const char* destination_uri, int aggregation_time_msec,
                   int buffer_size) {
  impl_ = nullptr;
  destination_file_ = nullptr;
  udp_connection_ = nullptr;
  if (strstr(destination_uri, kDestinationUDPScheme) == destination_uri) {
    const char* address = destination_uri + strlen(kDestinationUDPScheme);
    assert(strlen(address) > 0);
    const char* separator = strstr(address, ":");
    assert(separator != nullptr && separator > address);
    const char* port = separator + 1;
    assert(strlen(port) > 0);
    int port_num = atoi(port);
    assert(port_num);
    char addr[16];
    assert(separator - address > 0 && separator - address < 16);
    strncpy(addr, address, separator - address);
    addr[separator - address] = 0;
    addr[15] = 0;
    assert(strlen(addr) < 16);
    udp_connection_ = new UDPConnection(port_num, addr);
    assert(udp_connection_->IsConnected());
    if (buffer_size == 0) buffer_size = 508;  // UDP max packet size
    impl_ = new ProfilerImpl(
        std::bind(&Profiler::SendToUDP, udp_connection_, std::placeholders::_1,
                  std::placeholders::_2),
        buffer_size, aggregation_time_msec);
  } else if (strstr(destination_uri, kDestinationFileScheme) ==
             destination_uri) {
    const char* filename = destination_uri + strlen(kDestinationFileScheme);
    assert(strlen(filename) > 0);
    destination_file_ = std::fopen(filename, "wb");
    assert(destination_file_);
    if (buffer_size == 0) buffer_size = 4096;  // optimal buffer for disk write
    impl_ = new ProfilerImpl(
        std::bind(&Profiler::SendToFile, destination_file_,
                  std::placeholders::_1, std::placeholders::_2),
        buffer_size, aggregation_time_msec);
  }
}

Profiler::~Profiler() {
  if (destination_file_) {
    std::fclose(destination_file_);
    destination_file_ = nullptr;
  }
  if (udp_connection_) {
    delete udp_connection_;
    udp_connection_ = nullptr;
  }
  if (impl_) {
    delete impl_;
    impl_ = nullptr;
  }
}

Profiler::Profiler(Profiler&& o) { Move(std::move(o)); }

Profiler& Profiler::operator=(Profiler&& o) {
  Move(std::move(o));
  return *this;
}

void Profiler::GatherNSend() {
  if (impl_) impl_->GatherNSend();
}

int Profiler::GetAggregationTime() const {
  return impl_ ? impl_->GetAggregationTime() : 0;
}

int Profiler::GetBufferSize() const {
  return impl_ ? impl_->GetBufferSize() : 0;
}

void Profiler::Move(Profiler&& o) {
  impl_ = o.impl_;
  o.impl_ = nullptr;
  destination_file_ = o.destination_file_;
  o.destination_file_ = nullptr;
  udp_connection_ = o.udp_connection_;
  o.udp_connection_ = nullptr;
}

void Profiler::SendToUDP(UDPConnection* udp_connection, std::uint8_t* buffer,
                         int buffer_size) {
  assert(buffer_size > 0);
  udp_connection->Send(buffer, buffer_size);
}

void Profiler::SendToFile(std::FILE* destination_file, std::uint8_t* buffer,
                          int buffer_size) {
  assert(destination_file);
  assert(buffer_size > 0);
  auto written = std::fwrite(buffer, 1, buffer_size, destination_file);
  assert((int)written == buffer_size);
  (void)written;
}

}  // namespace pipert
