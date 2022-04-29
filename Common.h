#pragma once

#include <stdio.h>    /* printf */
#include <sys/time.h> /* gettimeofday, timeval (for timestamp in microsecond) */
#include <sys/timeb.h> /* ftime, timeb (for timestamp in millisecond) */
#include <time.h>      /* time_t, time (for timestamp in second) */

namespace ConcurrentHashMap {

typedef long long int timemilliseconds;

class noncopyable {
 public:
  noncopyable() {}

 private:
  noncopyable(const noncopyable&);
  noncopyable& operator=(const noncopyable&);
};

#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
  TypeName(const TypeName&);               \
  void operator=(const TypeName&);

timemilliseconds getMilliseconds();

}  // namespace ConcurrentHashMap
