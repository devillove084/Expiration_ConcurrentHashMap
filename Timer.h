#pragma once

#include <dispatch/dispatch.h>
#include <mach/boolean.h>
#include <signal.h>
#include <stdbool.h>
#include <stdlib.h>
#include <sys/errno.h>
#include <time.h>
#include <unistd.h>

#include <cstdio>
#include <mutex>
#include <utility>
#include <vector>

#include "Common.h"

struct itimerspec {
  struct timespec it_interval; /* timer period */
  struct timespec it_value;    /* timer expiration */
};

struct sigevent;

/* If used a lot, queue should probably be outside of this struct */
struct macos_timer {
  dispatch_queue_t tim_queue;
  dispatch_source_t tim_timer;
  void (*tim_func)(union sigval);
  void *tim_arg;
};

typedef struct macos_timer *timer_t;

static inline void _timer_cancel(void *arg) {
  struct macos_timer *tim = (struct macos_timer *)arg;
  dispatch_release(tim->tim_timer);
  dispatch_release(tim->tim_queue);
  tim->tim_timer = NULL;
  tim->tim_queue = NULL;
  free(tim);
}

static inline void _timer_handler(void *arg) {
  struct macos_timer *tim = (struct macos_timer *)arg;
  union sigval sv;

  sv.sival_ptr = tim->tim_arg;

  if (tim->tim_func != NULL) tim->tim_func(sv);
}

static inline int timer_create(clockid_t clockid, struct sigevent *sevp,
                               timer_t *timerid) {
  struct macos_timer *tim;

  *timerid = NULL;

  switch (clockid) {
    case CLOCK_REALTIME:

      /* What is implemented so far */
      if (sevp->sigev_notify != SIGEV_THREAD) {
        errno = ENOTSUP;
        return (-1);
      }

      tim = (struct macos_timer *)malloc(sizeof(struct macos_timer));
      if (tim == NULL) {
        errno = ENOMEM;
        return (-1);
      }

      tim->tim_queue = dispatch_queue_create("timerqueue", 0);
      tim->tim_timer = dispatch_source_create(DISPATCH_SOURCE_TYPE_TIMER, 0, 0,
                                              tim->tim_queue);

      tim->tim_func = sevp->sigev_notify_function;
      tim->tim_arg = sevp->sigev_value.sival_ptr;
      *timerid = tim;

      /* Opting to use pure C instead of Block versions */
      dispatch_set_context(tim->tim_timer, tim);
      dispatch_source_set_event_handler_f(tim->tim_timer, _timer_handler);
      dispatch_source_set_cancel_handler_f(tim->tim_timer, _timer_cancel);

      return (0);
    default:
      break;
  }

  errno = EINVAL;
  return (-1);
}

static inline int timer_settime(timer_t tim, int flags,
                                const struct itimerspec *its,
                                struct itimerspec *remainvalue) {
  if (tim != NULL) {
    /* Both zero, is disarm */
    if (its->it_value.tv_sec == 0 && its->it_value.tv_nsec == 0) {
      /* There's a comment about suspend count in Apple docs */
      dispatch_suspend(tim->tim_timer);
      return (0);
    }

    dispatch_time_t start;
    start =
        dispatch_time(DISPATCH_TIME_NOW, NSEC_PER_SEC * its->it_value.tv_sec +
                                             its->it_value.tv_nsec);
    dispatch_source_set_timer(
        tim->tim_timer, start,
        NSEC_PER_SEC * its->it_value.tv_sec + its->it_value.tv_nsec, 0);
    dispatch_resume(tim->tim_timer);
  }
  return (0);
}

static inline int timer_delete(timer_t tim) {
  /* Calls _timer_cancel() */
  if (tim != NULL) dispatch_source_cancel(tim->tim_timer);

  return (0);
}

namespace ConcurrentHashMap {
static void timerHandler(int sig, siginfo_t *si, void *uc);

struct TimerCall {
  timer_t *_timer_t;
  void (*func)(void *);
  void *para;
};

/// In linux, timer is one per process, thus
///  define it as a singleton class, for defining
///  multiple timers, try call create timer
class Timer : noncopyable {
 private:
  // Mutex * mutex;
  mutable std::recursive_mutex mutex;
  Timer() {
    // mutex = new Mutex();
  }
  static void timerHandler(int sig, siginfo_t *si, void *uc);

 public:
  static Timer &getInstance() {
    static Timer INSTANCE;
    return INSTANCE;
  }

  timer_t create(long expireMS, int intervalMS, void (*callbackFunc)(void *),
                 void *para);

  void remove(timer_t &timerID);

  int make(timer_t *timerID, int expireSecond, int intervalSecond) {
    // sigset_t mask;
    struct sigevent te;
    struct itimerspec its;
    struct sigaction sa;
    int sigNo = SIGUSR1;

    /* Set up signal handler. */
    sa.sa_flags = SA_SIGINFO;
    sa.sa_sigaction = timerHandler;
    sigemptyset(&sa.sa_mask);
    if (sigaction(sigNo, &sa, NULL) == -1) {
      perror("sigaction");
    }

    /* Set and enable alarm */
    te.sigev_notify = SIGEV_SIGNAL;
    te.sigev_signo = sigNo;
    te.sigev_value.sival_ptr = timerID;
    timer_create(CLOCK_REALTIME, &te, timerID);

    its.it_interval.tv_sec = intervalSecond;
    its.it_interval.tv_nsec = 0;
    its.it_value.tv_sec = expireSecond;
    its.it_value.tv_nsec = 0;
    timer_settime(*timerID, 0, &its, NULL);

    return 1;
  }

  ~Timer() {}
};
}  // namespace ConcurrentHashMap