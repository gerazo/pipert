#include "SteadyHeap.h"
#include "gtest/gtest.h"

#include <algorithm>
#include <functional>
#include <vector>

TEST(SteadyHeap, SteadyHeapReserveTest) {
  pipert::SteadyHeap<int, std::function<bool(const int&, const int&)>> sh{
      std::less<int>()};
  EXPECT_TRUE(sh.empty());
  EXPECT_EQ(sh.capacity(), 0);
  EXPECT_EQ(sh.size(), 0);
  sh.reserve(1);
  EXPECT_TRUE(sh.empty());
  EXPECT_EQ(sh.capacity(), 1);
  EXPECT_EQ(sh.size(), 0);
  sh.reserve(0);
  EXPECT_TRUE(sh.empty());
  EXPECT_EQ(sh.capacity(), 1);
  EXPECT_EQ(sh.size(), 0);
}

TEST(SteadyHeap, SteadyHeapPushPopTest) {
  pipert::SteadyHeap<int, std::function<bool(const int&, const int&)>> sh{
      std::less<int>()};
  const std::vector<int> v{3, 2, 4, 8, 6, 7};
  sh.reserve(v.size());
  EXPECT_EQ(sh.size(), 0);
  for (unsigned long i = 0; i < v.size(); ++i) {
    EXPECT_EQ(sh.size(), i);
    sh.push_heap(v[i]);
  }
  EXPECT_EQ(sh.size(), v.size());
  for (unsigned long i = v.size(); i > 0; --i) {
    EXPECT_EQ(sh.size(), i);
    sh.pop_heap();
  }
  EXPECT_EQ(sh.size(), 0);
}

TEST(SteadyHeap, SteadyHeapOrderingTest) {
  pipert::SteadyHeap<int, std::function<bool(const int&, const int&)>> sh{
      std::less<int>()};
  const std::vector<int> v{3, 2, 4, 8, 6, 7};
  std::vector<int> descend{v};
  std::sort(descend.begin(), descend.end(), std::greater<int>());
  sh.reserve(v.size());
  for (unsigned long i = 0; i < v.size(); ++i) {
    sh.push_heap(v[i]);
    EXPECT_EQ(sh.front(), *std::max_element(v.begin(), v.begin() + i + 1));
  }
  for (unsigned long i = 0; i < v.size(); ++i) {
    EXPECT_EQ(sh.front(), descend[i]);
    sh.pop_heap();
  }
}

TEST(SteadyHeap, SteadyHeapForcePopTest) {
  pipert::SteadyHeap<int, std::function<bool(const int&, const int&)>> sh{
      std::less<int>()};
  const std::vector<int> v{3, 2, 4, 8, 6, 7};
  for (unsigned long i = 0; i < v.size(); ++i) {
    sh.push_heap(v[i]);
  }
  const int min = *std::min_element(sh.begin(), sh.end());
  const int max = *std::max_element(sh.begin(), sh.end());
  EXPECT_NE(std::min_element(sh.begin(), sh.end()),
            std::max_element(sh.begin(), sh.end()));
  EXPECT_EQ(sh.size(), v.size());
  EXPECT_NE(std::find(sh.begin(), sh.end(), min), sh.end());
  EXPECT_EQ(sh.front(), max);
  sh.force_pop_heap(min);
  EXPECT_EQ(sh.size(), v.size() - 1);
  EXPECT_EQ(std::find(sh.begin(), sh.end(), min), sh.end());
  EXPECT_EQ(sh.front(), max);
}