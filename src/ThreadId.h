#ifndef _THREADID_H_
#define _THREADID_H_

#include <atomic>
#include <cstdint>

namespace pipert {

/// Helps in tagging and identifying the currently running thread.
///
/// The implementation uses `thread_local` variables which are supposed to have
/// better performing implementation than identification by using OS calls.
/// This class is needed by the monitoring / logging system to be able to log
/// which thread is doing what.
/// This information is supposed to give the connection between the source
/// and sink user _Nodes_ and connected Channel objects when data is going
/// through a transformation done by user code in a _Node_.
class ThreadId {
 public:
  /// Returns the ThreadId of the currently running thread.
  ///
  /// If the currently running thread was never tagged before
  /// (see IsCurrentThreadUntagged()), it will automatically tag it
  /// as _auto-tagged_ and return the new tag.
  /// This way, all threads are tagged but can be later classified by
  /// IsHandTagged() and IsAutoTagged() functions.
  /// \return ThreadId of the currently running thread.
  ///         The returned object cannot be untagged, but it can be either
  ///         previously _hand tagged_ or _auto tagged_.
  static ThreadId GetCurrentThread();

  /// This explicitly tags the currently running thread as _hand tagged_,
  /// so we can see that this is ours later on.
  /// \pre The current thread needs to be _untagged_.
  ///      It would lead to errors if retagging was possible.
  static void TagCurrentThread();

  /// Returns true if the current thread was not tagged or autotagged before.
  /// See GetCurrentThread() for details on autotagging.
  static bool IsCurrentThreadUntagged();

  ThreadId(const ThreadId&) = default;
  ThreadId& operator=(const ThreadId&) = default;

  /// Returns true if the thread was tagged by explicitly calling
  /// TagCurrentThread().
  bool IsHandTagged() const;

  /// Returns true if this thread was tagged by calling GetCurrentThread()
  /// on an untagged thread, so it was automatically tagged.
  bool IsAutoTagged() const;

  /// Get ID value for serialization purposes.
  /// \return A value which is at least 32-bits (safe to convert to 32-bit).
  int_least32_t GetIdForSerialization();

 private:
  /// Above this, we have tagged IDs, below we have autotagged ones.
  static const int kUntaggedValue = 0;

  /// This is the ID of the currently running thread.
  thread_local static int current_thread_id_;

  static std::atomic<int> hand_tag_next_;
  static std::atomic<int> auto_tag_next_;

  ThreadId(int thread_id);

  int thread_id_;  ///< ID of a thread which this object represents.
};

}  // namespace pipert

#endif  // _THREADID_H_