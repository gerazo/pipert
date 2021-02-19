#include "pipert/Profiler.h"

#include <cassert>
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
  if (strstr(destination_uri, kDestinationUDPScheme) == destination_uri) {
    const char* address = destination_uri + strlen(kDestinationUDPScheme);
    assert(strlen(address) > 0);
    assert(strstr(address, ":"));
    const char* port = strstr(address, ":") + 1;
    std::string port_str=std::string(port);
    assert(strlen(port) > 0);
    std::string adress_str_= std::string(address);
    std::size_t pos_ = adress_str_.find(":");
    std::string ip_=adress_str_.substr(0,pos_);
    assert(strlen(ip_.c_str()) > 0);
    char *ip_char_ = new char[ip_.length() + 1];
    strcpy(ip_char_, ip_.c_str());
    connection_=UDPConnection(static_cast<std::uint16_t>(std::stoi(port_str)),ip_char_);
    delete [] ip_char_;
    // TODO Remove trailing :portnumber from address
    // TODO Convert port to number
    // TODO Init UDP communication here
    // TODO Make sure it is handled in destructor and move operations
    if (buffer_size == 0) buffer_size = 508;  // UDP max packet size
    impl_ = new ProfilerImpl(
        std::bind(&Profiler::SendToUDP, 0, std::placeholders::_1,
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
  // TODO destroy UDP connection if needed
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
  // TODO Move UDP connection correctly
  o.connection_=connection_;
}

void Profiler::SendToUDP(int /*socket*/,
                         std::uint8_t* /*buffer*/, int /*buffer_size*/) {
  // TODO Send data through void
  assert(buffer_size> 0);
  connection_.openCoccection();
  connection_.send(buffer,buffer_size);
  connection_.closeConnection();
}

void Profiler::SendToFile(std::FILE* destination_file,
                         std::uint8_t* buffer, int buffer_size) {
  assert(destination_file);
  assert(buffer_size > 0);
  auto written = std::fwrite(buffer, 1, buffer_size, destination_file);
  assert((int)written == buffer_size);
  (void)written;
}

}  // namespace pipert
