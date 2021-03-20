#ifndef _PROFILER_H_
#define _PROFILER_H_

#include <cstdint>
#include <cstdio>

namespace pipert {

class ProfilerImpl;
class Scheduler;
class UDPConnection;

/// Controlling object of all profiling and monitoring functionality.
///
/// This object represents the subsystem which collects, aggregates, serializes
/// and sends data to a destination which accepts logs.
/// It is using the `NullObject` pattern, so a Profiler without destination
/// will do completely nothing.
/// This object is passed to the Scheduler which governs this internally.
/// The destination is given by a URI which supports more destination types.
///
/// This object serializes data into network byte order.
/// Each packet starts with a header:
/// `"DGRP"<null-terminated string of data group/channel name>`
/// then once for each log serialization pass
/// `"SEND"<null-terminated string of sender object name>`
/// then arbitrary number of aggregated data as
/// `"LOGA"<null-terminated string of event type name>`
/// 32 bytes: `<4-byte-int log count> <4-byte-int time passed in microseconds>`
/// `<8-byte-double minimum value> <8-byte-double maximum value>`
/// `<8-byte-double average value>`
class Profiler {
 public:
  /// Special NullObject constant representing no destination, no monitoring.
  static const char kNoDestination[];

  /// UDP protocol scheme for destination URI
  static const char kDestinationUDPScheme[];

  /// File protocol scheme for destination URI
  static const char kDestinationFileScheme[];

  /// Constructs an object.
  /// \param destination_uri Describes the destination and the used protocol.
  ///                        `"udp:<server>:<port>"` sends data to a UDP port.
  ///                        `"file:[/][path/]<file>"` writes to a file.
  /// \param aggregation_time_msec Time in milliseconds to wait before
  ///                              gathering monitoring data again.
  ///                              This value determines how often aggregated
  ///                              log data is sent to the log processor.
  ///                              (Background log collection is switched on.)
  ///                              Default means not to collect periodically.
  ///                              (Background log collection is switched off.)
  /// \param buffer_size Size of buffer which is filled to be sent at once.
  ///                    Default value will choose the default for the selected
  ///                    protocol. (This is max packet size for UDP.)
  Profiler(const char* destination_uri = kNoDestination,
           int aggregation_time_msec = 0, int buffer_size = 0);
  ~Profiler();
  Profiler(const Profiler&) = delete;
  Profiler& operator=(const Profiler&) = delete;
  Profiler(Profiler&& o);
  Profiler& operator=(Profiler&& o);

  /// Gathers all logged data and sends it to its destination.
  ///
  /// This function is called automatically `aggregation_time_msec_`
  /// milliseconds after the last log data gathering.
  /// Calling this manually makes sense when automatic background aggregation
  /// is switched off by setting `aggregation_time_msec_` to 0.
  void GatherNSend();

  /// Returns time in milliseconds between log collections.
  int GetAggregationTime() const;

  /// Returns buffer size used for collecting log data.
  int GetBufferSize() const;

 private:
  void Move(Profiler&& o);
  static void SendToUDP(UDPConnection* udp_connection, std::uint8_t* buffer,int buffer_size);
  static void SendToFile(std::FILE* destination_file, std::uint8_t* buffer,int buffer_size);


  friend Scheduler;
  ProfilerImpl* impl_;
  std::FILE* destination_file_;
  UDPConnection* udp_connection_;
};

}  // namespace pipert

#endif  // _PROFILER_H_
