#include "thread.h"

#ifdef __WIN32
#include <windows.h>
#else
#include <pthread.h>
#endif

int
txbe_thread_create (txbe_thread *thread, txbe_thread_func func, void *arg)
{
#ifdef __WIN32
  *thread = CreateThread (NULL, 0, func, arg, 0, NULL);
  return *thread != NULL ? 0 : -1;
#else
  return pthread_create (thread, NULL, func, arg);
#endif
}

int
txbe_thread_join (txbe_thread thread)
{
#ifdef __WIN32
  WaitForSingleObject (thread, INFINITE);
  CloseHandle (thread);
  return 0;
#else
  return pthread_join (thread, 0);
#endif
}