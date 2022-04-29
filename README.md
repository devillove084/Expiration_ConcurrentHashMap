# Expiration_ConcurrentHashMap

## 简介：

本项目简单实现了一个Expired-Key Value的并发HashMap，基本功能如下：

1. 支持多线程并发操作，保证线程安全；
2. 支持记录变化的时间，可以按照时间顺序获取key；
3. 支持删除某个时间戳范围内的数据；
4. 设置数据过期时间，数据过期后可以删除，可以持久化
5. 支持实时处理kv容器中的数据，对数据进行监控和处理

上述2 3 4，都是基于过期时间戳的回调函数实现的，具体形式如下：

```c++
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
```

在创建HashMap实例的时候获取时间，并且设置超时后的回调函数，灵活性足够高，可以单独记录时间变化下的kv，可以设置到期后删除某个范围内的kv数据，可以设置过期后，对kv的保存或者删除。并且可以针对容器中实时变化的kv进行监控，姑且称为回调的回调(异步)，在某个处理周期中对容器内部的kv结构进行分片，可以按时间分片，可以按kv分片，每次处理一个回调(周期)的数据。



## 使用

### 平台

时间关系，这里仅在MacOS M1下进行测试，使用编译器版本如下：

>Homebrew clang version 13.0.1
>Target: arm64-apple-darwin21.4.0
>Thread model: posix
>InstalledDir: /opt/homebrew/opt/llvm/bin

### 依赖

主要依赖gtest，执行

> brew install gtest

### 编译

> git clone https://github.com/devillove084/Expiration_ConcurrentHashMap.git
>
> mkdir build & cd build
>
> cmake ..
>
> make -j4

### 执行

> ./interview

### 输出

```
[==========] Running 3 tests from 1 test suite.
[----------] Global test environment set-up.
[----------] 3 tests from ConcurrentHashMapTest
[ RUN      ] ConcurrentHashMapTest.SizeTest
[       OK ] ConcurrentHashMapTest.SizeTest (0 ms)
[ RUN      ] ConcurrentHashMapTest.OnePutTest
[       OK ] ConcurrentHashMapTest.OnePutTest (0 ms)
[ RUN      ] ConcurrentHashMapTest.ClearTest
```



