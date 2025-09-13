#include "thread.h"

#include <pthread.h>

int
txbe_thread_create (txbe_thread *thread, txbe_thread_func func, void *arg)
{
  return pthread_create (thread, NULL, func, arg);
}

int
txbe_thread_join (txbe_thread thread)
{
  return pthread_join (thread, 0);
}