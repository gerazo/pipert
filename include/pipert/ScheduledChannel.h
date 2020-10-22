#ifndef _SCHEDULEDCHANNEL_H_
#define _SCHEDULEDCHANNEL_H_

#include <functional>
#include "pipert/Channel.h"

namespace pipert {

/// A Channel which packets are automatically scheduled for processing by a
/// worker thread pool.
///
/// A ScheduledChannel has all its enqueued Packet objects queued in the
/// owning Scheduler of the system as well.
/// This means that depending on the Scheduler settings, these packets will be
/// processed by an available thread coming from the worker thread pool
/// of the Scheduler.
/// See Scheduler for details.
///
/// \tparam T Same as in case of `Channel<T>`.
template <class T>
class ScheduledChannel : public Channel<T> {
 public:
  /// This is the callback function type which is called by the worker thread
  /// when a new Packet is scheduled for processing.
  /// It should contain the processing function for T data type.
  /// Its parameter is a stub pointing to the selected Packet object.
  using Callback = std::function<void(PacketToProcess<T>)>;

  /// (_Internal implementation_) Translates from C-style callback
  /// into C++-style user registered function object.
  static void CallbackTranslator(ChannelBase* this_channel, PacketBase* packet);

  /// A Channel can only be created directly from the implementation.
  /// Call Scheduler::CreateScheduledChannel() to create one.
  ScheduledChannel(ChannelImpl* impl, Callback processing_fun);

  ScheduledChannel(ScheduledChannel&&) = default;
  ScheduledChannel& operator=(ScheduledChannel&&) = default;

 private:
  /// The callback containing the registered callback function.
  Callback processing_fun_;
};

template <class T>
ScheduledChannel<T>::ScheduledChannel(ChannelImpl* impl,
                                      Callback processing_fun)
    : Channel<T>(impl), processing_fun_(processing_fun) {}

template <class T>
void ScheduledChannel<T>::CallbackTranslator(ChannelBase* this_channel,
                                             PacketBase* packet) {
  assert(this_channel);
  ScheduledChannel<T>* my_this =
      reinterpret_cast<ScheduledChannel<T>*>(this_channel);
  assert(my_this);
  my_this->processing_fun_(
      PacketToProcess<T>(reinterpret_cast<Packet<T>*>(packet), my_this));
}

}  // namespace pipert

#endif  //_SCHEDULEDCHANNEL_H_