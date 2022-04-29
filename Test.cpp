#include <gtest/gtest.h>

#include <cstddef>
#include <iostream>
#include <vector>

#include "Hashtable.h"

void callbck_func(unsigned long& s) {
  std::cout << "The key is expired: " << s << std::endl;
}

TEST(ConcurrentHashMapTest, SizeTest) {
  ConcurrentHashMap::Timer& timer = ConcurrentHashMap::Timer::getInstance();
  ConcurrentHashMap::Hashtable<unsigned long, unsigned long> table(
      100, 0.75f, 5, callbck_func);

  std::vector<unsigned long> keys;
  for (size_t i = 0; i < 10; ++i) {
    table.put(i, i);
  }
  EXPECT_EQ(table.size(), 10);
}

TEST(ConcurrentHashMapTest, OnePutTest) {
  ConcurrentHashMap::Timer& timer = ConcurrentHashMap::Timer::getInstance();
  ConcurrentHashMap::Hashtable<unsigned long, unsigned long> table(
      100, 0.75f, 5, callbck_func);

  table.put(1, 1);
  EXPECT_EQ(table.contain(1), true);
}

TEST(ConcurrentHashMapTest, ClearTest) {
  ConcurrentHashMap::Timer& timer = ConcurrentHashMap::Timer::getInstance();
  ConcurrentHashMap::Hashtable<unsigned long, unsigned long> table(
      100, 0.75f, 5, callbck_func);

  for (size_t i = 0; i < 100; ++i) {
    table.put(i, i);
  }
  table.clear();
  EXPECT_EQ(table.size(), 0);
}

int main(int argc, char* argv[]) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}