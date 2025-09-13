#ifndef __txbe_thread_h__
#define __txbe_thread_h__

// ass windows ew
#ifdef _WIN32
#error "This software is not supported on windows"
#else
#include <pthread.h>
#include <unistd.h>
typedef pthread_t txbe_thread;
typedef void *txbe_thread_ret;
#define THREAD txbe_thread_ret
#define THREAD_RET_NULL ((void *)0)
#define txbe_thread_sleep(ms) usleep (ms * 1000)
#endif

typedef txbe_thread_ret (*txbe_thread_func) (void *);

int txbe_thread_create (txbe_thread *, txbe_thread_func, void *);
int txbe_thread_join (txbe_thread);

#endif