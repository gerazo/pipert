#ifndef _PROFILER_IMPL_H_
#define _PROFILER_IMPL_H_

#include <atomic>
#include <cstdint>
#include <functional>
#include <thread>
#include <vector>
#include "pipert/UDPConnection.h"

namespace pipert {

class ProfileData;

/// (_Part of internal implementation_.)
///
/// The internal implementation of the Profiler object which is responsible
/// for managing all ProfileData objects connected to channels.
/// It launches its own thread and periodically empties the collected
/// monitoring information by sending it to a log processor.
/// The launched thread runs independently and can block while data is sent.
/// The thread wakes up in the background continuously to empty all
/// ProfileData objects until it goes to sleep and waits for the next round.
class ProfilerImpl {
 public:
  /// The callback function type which is used as the interface towards the
  /// object which will actually send serialized buffers of log data.
  using SenderCallback = std::function<void(std::uint8_t*, int)>;

  /// Constructs an object.
  /// \param sender_callback Function to be called with buffers of logged data.
  /// \param buffer_size Size of buffer which is filled to be sent at once.
  /// \param aggregation_time_msec Time in milliseconds to wait before
  ///                              gathering monitoring data again.
  ///                              (Background log collection is switched on.)
  ///                              Default means not to collect periodically.
  ///                              (Background log collection is switched off.)
  ProfilerImpl(SenderCallback sender_callback, int buffer_size,
               int aggregation_time_msec = 0);
  ~ProfilerImpl();
  ProfilerImpl(const ProfilerImpl&) = delete;
  ProfilerImpl& operator=(const ProfilerImpl&) = delete;

  /// Returns time in milliseconds between log collections.
  int GetAggregationTime() const { return aggregation_time_msec_; }

  /// Returns buffer size used for collecting log data.
  int GetBufferSize() const { return buffer_size_; }

  /// Adds ProfileData object to be regularly emptied by this Profiler object.
  /// \param profile_data Points to the ProfileData to be added.
  /// \pre This should be used in preparation mode and not when the Scheduler
  ///      is running. (Normally Channel objects are added before running.)
  ///      The collector thread should not run.
  void AddProfileData(ProfileData* profile_data);

  /// Removes ProfileData object registered to this Profiler.
  /// \param profile_data Points to the ProfileData to be removed.
  /// \pre This should be used in preparation mode and not when the Scheduler
  ///      is running. (Normally Channel objects are removed after running.)
  ///      The collector thread should not run.
  void RemoveProfileData(ProfileData* profile_data);

  /// Start log collector thread (if it is allowed by aggregation time).
  /// \pre The collector thread should not run.
  void Start();

  /// Stop log collector thread (if it is allowed by aggregation time).
  /// \pre The collector thread should run.
  void Stop();

  /// Runs a gather-and-send action manually on the current thread.
  ///
  /// This function goes through all ProfileData objects, fills up
  /// the buffer with data and sends it by calling `sender_callback_`.
  /// It waits for the function to return and goes on until there is data
  /// left in any of the ProfileData objects.
  /// It does one pass and returns.
  /// This function is called automatically `aggregation_time_msec_`
  /// milliseconds after the last log data gathering.
  /// Calling this manually makes sense when automatic background aggregation
  /// is switched off by setting `aggregation_time_msec_` to 0.
  void GatherNSend();


 private:
  void SleepNWork();

  SenderCallback sender_callback_;
  int buffer_size_;
  int aggregation_time_msec_;
  std::thread worker_thread_;
  std::atomic<bool> worker_shutdown_;
  std::uint8_t* buffer_;
  std::vector<ProfileData*> profile_data_;
  std::atomic<int> next_profile_data_;
};

}  // namespace pipert

#endif  // _PROFILER_IMPL_H_
