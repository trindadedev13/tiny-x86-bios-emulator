/**
 * Ebios-Backend Template.
 */
#include "event.h"

void txbe_backend_myback_event_process(struct txbe_event* e) {
  /** you should implement these cases */
  switch (e->type) {
    case EVENT_START: break;
    case EVENT_END:   break;
    case EVENT_UNKNOWN_OPCODE:      break;
    case EVENT_SET_VIDEOMODE:       break;
    case EVENT_SET_TEXTMODE_CURSOR: break;
    case EVENT_SET_CURSOR_POS: break;
    case EVENT_GET_CURSOR_POS: break;
    case EVENT_READ_LIGHTPEN_POS:  break;
    case EVENT_SELECT_DISPLAYPAGE: break;
    case EVENT_SCROLL_UP:   break;
    case EVENT_SCROLL_DOWN: break;
    case EVENT_READ_CHARACTER:       break;
    case EVENT_WRITE_CHARACTER:      break;
    case EVENT_WRITE_CHARACTER_ONLY: break;
    case EVENT_SET_PALETTE: break;
    case EVENT_WRITE_PIXEL: break;
    case EVENT_READ_PIXEL:  break;
    case EVENT_TELETYPE_OUTPUT: break;
    case EVENT_GET_VIDEOMODE:   break;
    case EVENT_VIDEO_SERVICES:  break;
  }
}

EVENT_QUEUE_PROCESS_FUNC() {
  struct txbe_event e;
  while (txbe_event_pool(&e) == 0) {
    txbe_backend_myback_event_process(&e);
  }
}