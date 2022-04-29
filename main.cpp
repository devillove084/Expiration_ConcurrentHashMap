#include <cstddef>
#include <iostream>
#include <vector>

#include "Hashtable.h"

void callbck_func(unsigned long& s) {
  std::cout << "The key is expired: " << s << std::endl;
}

int main() {
  ConcurrentHashMap::Timer& timer = ConcurrentHashMap::Timer::getInstance();
  ConcurrentHashMap::Hashtable<unsigned long, unsigned long> table(
      100, 0.75f, 5, callbck_func);

  std::vector<unsigned long> keys;
  for (size_t i = 0; i < 10; ++i) {
    table.put(i, i);
  }
  std::cout << table.size() << std::endl;

  while (true) {
    sleep(1);
  }
}
