#ifndef _THREADID_H_
#define _THREADID_H_

#include <atomic>
#include <cstdint>

namespace pipert {

/// (Part of internal implementation.)
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

  /// We can set the current thread to be identified temporarily,
  /// while it is working on something.
  /// \pre The current thread needs to be already manually _tagged_.
  /// \param thread_name Pointer to name to be set (only pointer is stored).
  static void SetNameOfCurrentThread(const char* thread_name);

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
  int_least32_t GetIdForSerialization() const;

  /// Get the set name of the thread.
  /// \return Pointer to the name of the thread.
  const char* GetName() const;

 private:
  /// Above this, we have tagged IDs, below we have autotagged ones.
  static const int kUntaggedValue = 0;

  /// This is the ID of the currently running thread.
  thread_local static int current_thread_id_;

  /// This is the name of the currently running thread which can be changed.
  thread_local static const char* current_thread_name_;

  static std::atomic<int> hand_tag_next_;
  static std::atomic<int> auto_tag_next_;

  ThreadId(int thread_id, const char* thread_name);

  int thread_id_;            ///< ID of a thread which this object represents.
  const char* thread_name_;  ///< The changeable name of the thread.
};

}  // namespace pipert

#endif  // _THREADID_H_