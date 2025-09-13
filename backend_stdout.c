/**
 * Ebios Standard Output Backend.
 */
#include "event.h"

#include <stdio.h>
#include <stdlib.h>

#include "cpu.h"

static inline int
sys_clear ()
{
  return system ("clear");
}

void
txbe_backend_stdout_event_process (struct txbe_event *e)
{
  switch (e->type)
    {
    case EVENT_START:
      sys_clear ();
      break;
    case EVENT_END:
      sys_clear ();
      break;
    case EVENT_UNKNOWN_OPCODE:
      fprintf (stderr, "Unknown opcode 0x%02X at CS:IP = %04X:%04X\n",
               e->unknown_opcode.opcode, cpu.cs, cpu.ip);
      fflush (stderr);
      break;
    case EVENT_SET_VIDEOMODE:
    case EVENT_SET_TEXTMODE_CURSOR:
    case EVENT_SET_CURSOR_POS:
    case EVENT_GET_CURSOR_POS:
    case EVENT_READ_LIGHTPEN_POS:
    case EVENT_SELECT_DISPLAYPAGE:
    case EVENT_SCROLL_UP:
    case EVENT_SCROLL_DOWN:
    case EVENT_READ_CHARACTER:
    case EVENT_SET_PALETTE:
    case EVENT_WRITE_PIXEL:
    case EVENT_READ_PIXEL:
    case EVENT_GET_VIDEOMODE:
    case EVENT_VIDEO_SERVICES:
      break;

    case EVENT_WRITE_CHARACTER:
      for (uint16_t i = 0; i < e->write_character.count; i++)
        {
          putchar (e->write_character.character);
        }
      fflush (stdout);
      break;

    case EVENT_WRITE_CHARACTER_ONLY:
      for (uint16_t i = 0; i < e->write_character_only.count; i++)
        {
          putchar (e->write_character_only.character);
        }
      fflush (stdout);
      break;

    case EVENT_TELETYPE_OUTPUT:
      putchar (e->teletype_output.character);
      fflush (stdout);
      break;
    }
}

EVENT_QUEUE_PROCESS_FUNC ()
{
  struct txbe_event e;
  while (txbe_event_pool (&e) == 0)
    {
      txbe_backend_stdout_event_process (&e);
    }
}