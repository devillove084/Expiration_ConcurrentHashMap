#include "Timer.h"

#include <vector>

std::vector<ConcurrentHashMap::TimerCall> vec;

void ConcurrentHashMap::Timer::timerHandler(int sig, siginfo_t *si, void *uc) {
  timer_t *tidp = (timer_t *)si->si_value.sival_ptr;
  printf("time handler is triggered\n");
  for (std::vector<TimerCall>::iterator itr = vec.begin(); itr != vec.end();
       ++itr) {
    timer_t id = *((*itr)._timer_t);
    if (id == *tidp) {
      printf("execute command\n");
      (*itr).func((*itr).para);
    }
  }
}

timer_t ConcurrentHashMap::Timer::create(long expireMS, int intervalMS,
                                         void (*callbackFunc)(void *),
                                         void *para) {
  timer_t id;

  make(&id, expireMS, intervalMS);
  TimerCall call;
  call._timer_t = &id;
  call.func = callbackFunc;
  call.para = para;
  vec.push_back(call);
  return id;
}

void ConcurrentHashMap::Timer::remove(timer_t &timerID) {
  size_t j = 0;
  bool found = false;
  for (std::vector<TimerCall>::iterator itr = vec.begin(); itr != vec.end();
       ++itr) {
    timer_t id = *((*itr)._timer_t);
    if (id == timerID) {
      found = true;
      break;
    }
    j++;
  }

  if (found) {
    vec.erase(vec.begin() + j);
    int ret = timer_delete(timerID);
    if (ret != 0) printf("delete timer failed , ret = %d", ret);
  }
}