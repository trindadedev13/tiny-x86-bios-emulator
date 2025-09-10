#include "event.h"

static struct txbe_event queue[EVENT_QUEUE_SIZE] = {};
static int queue_start = 0;
static int queue_end = 0;
static int queue_size = 0;

void
txbe_event_push (struct txbe_event *e)
{
  if (queue_size < EVENT_QUEUE_SIZE)
    {
      queue[queue_end] = *e;
      queue_end = (queue_end + 1) % EVENT_QUEUE_SIZE;
      queue_size++;
    }
}

int
txbe_event_pool (struct txbe_event *e)
{
  if (queue_size == 0)
    return 1;
  *e = queue[queue_start];
  queue_start = (queue_start + 1) % EVENT_QUEUE_SIZE;
  queue_size--;
  return 0;
}