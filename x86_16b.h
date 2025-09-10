#ifndef __txbe_x86_16_h__
#define __txbe_x86_16_h__

#include <stdint.h>

#include "cpu.h"

/** Right Byte Regs */
static inline uint8_t
txbe_x86_16_getah ()
{
  return cpu.ax >> 8;
}

static inline uint8_t
txbe_x86_16_getbh ()
{
  return cpu.bx >> 8;
}

static inline uint8_t
txbe_x86_16_getch ()
{
  return cpu.cx >> 8;
}

static inline uint8_t
txbe_x86_16_getdh ()
{
  return cpu.dx >> 8;
}

/** Low Byte Regs */
static inline uint8_t
txbe_x86_16_getal ()
{
  return cpu.ax & 0xFF;
}

static inline uint8_t
txbe_x86_16_getbl ()
{
  return cpu.bx & 0xFF;
}

static inline uint8_t
txbe_x86_16_getcl ()
{
  return cpu.cx & 0xFF;
}

static inline uint8_t
txbe_x86_16_getdl ()
{
  return cpu.dx & 0xFF;
}

#endif