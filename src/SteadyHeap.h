#ifndef _STEADY_HEAP_H_
#define _STEADY_HEAP_H_

#include <algorithm>
#include <cassert>
#include <vector>

namespace pipert {

/// A SteadyHeap wraps the functionality of a heap that can be restrained to
/// occupy a fix-sized space in memory. It does this by encapsulating a vector
/// and by providing only the interface to reserve the maximum amount of memory
/// this heap is allowed to consume, as well as providing methods for handling
/// its contents as a heap. This container wrapper is specially adapted to our
/// needs, and contains only the functions the library uses. It does not give a
/// complete interface for a generic heap, and the implementation is expected to
/// be extended when new requirements arise in the library.
///
/// \tparam T The type of data the heap will contain as its elements.
/// \tparam StrictOrdering The type of the comparison operation that will be
/// used to determine the positioning of elements in the heap. It should satisfy
/// the requirements of _Compare_, meaning that the return value of the
/// comparison function, when contextually converted to `bool`, should yield
/// `true` if its first argument appears before the second in the strict weak
/// ordering induced by the type of its parameters. Comparison using less-than
/// yields a max heap, and greater-than yields a min heap.
template <typename T, typename StrictOrdering>
class SteadyHeap {
 private:
  /// The fixed-size space in memory that the heap is realized in.
  std::vector<T> _heap;
  /// The strict ordering that is used to compare elements inside the heap.
  StrictOrdering _ordering;

 public:
  /// Construct a SteadyHeap with a strict ordering.
  ///
  /// \param ordering See the template parameter StrictOrdering for more
  /// information. Probably best implemented as a std::function wrapper.
  SteadyHeap(StrictOrdering ordering) : _ordering(ordering) {}

  /// \return Whether the container is empty.
  bool empty() const { return _heap.empty(); }

  /// \return The number of elements in the container.
  std::size_t size() const { return _heap.size(); }

  /// \return The reserved size of the container.
  std::size_t capacity() const { return _heap.capacity(); }

  /// \return An unmodifiable reference to the element at the top of the heap.
  const T& front() const { return _heap.front(); }

  /// Reserves the space in memory the heap is allowed to occupy at its fullest.
  void reserve(std::size_t n) { _heap.reserve(n); }

  /// Finds an element in the heap using `std::find`.
  typename std::vector<T>::iterator find(const T& value) {
    return std::find(_heap.begin(), _heap.end(), value);
  }

  /// Counts the number of occurrences of an element using `std::count`.
  std::ptrdiff_t count(const T& value) const {
    return std::count(_heap.begin(), _heap.end(), value);
  }

  /// Push an element onto the heap.
  void push_heap(T& value) {
    _heap.push_back(value);
    std::push_heap(_heap.begin(), _heap.end(), _ordering);
  }

  /// Pop the element at the top of the heap.
  void pop_heap() {
    std::pop_heap(_heap.begin(), _heap.end(), _ordering);
    _heap.pop_back();
  }

  /// A special method that forces an element onto the top of the heap, and then
  /// pops it from there. This is useful when a modified element has to be
  /// removed from (and later reinserted into) the heap, because the
  /// modification could have affected its position inside the heap.
  void force_pop_heap(const T& value) {
    force_to_top(value);
    pop_heap();
  }

 private:
  /// Forces an element to the top of the heap. Used when an element has to be
  /// force-popped from the heap because it was modified, see
  /// `SteadyHeap::force_pop_heap(T)`.
  void force_to_top(const T& value) {
    typename std::vector<T>::iterator it = find(value);
    assert(it != _heap.end());
    int i = &(*it) - &_heap[0];
    while (i > 0) {
      int j = (i - 1) / 2;
      assert(j < i);
      std::swap(_heap[i], _heap[j]);
      i = j;
    }
  }
};

}  // namespace pipert

#endif  //_STEADY_HEAP_H_