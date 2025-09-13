#ifndef __txbe_emu_h__
#define __txbe_emu_h__

#include <stdint.h>
#include <pthread.h>

enum txbe_emu_state
{
  EMU_STOPPED = 0,
  EMU_RUNNING = 1,
  EMU_PAUSED = 2,
};

void txbe_emu_backend_start ();
void txbe_emu_backend_end ();
void txbe_emu_start ();
void txbe_emu_pause ();
void txbe_emu_resume ();
void txbe_emu_stop ();
void* txbe_emu_thread (void *);
void txbe_emu_emulate ();

void txbe_emu_handle_interrupt (uint8_t);
uint16_t txbe_emu_calc_rm16 (uint8_t);

int txbe_emu_emulate_next (uint16_t);

#endif