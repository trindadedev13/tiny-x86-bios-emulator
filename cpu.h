#ifndef __txbe_cpu_h__
#define __txbe_cpu_h__

#include <stdint.h>

#define EBIOS_RAM_SIZE 0x1000000

struct txbe_cpu
{
  uint16_t ax, bx, cx, dx;
  uint16_t si, di, bp, sp;
  uint16_t cs, ds, es, ss;
  uint16_t ip, flags;
};

#define FLAG_CF 0x0001 // carry
#define FLAG_PF 0x0004 // parity
#define FLAG_AF 0x0010 // adjust
#define FLAG_ZF 0x0040 // zero
#define FLAG_SF 0x0080 // sign
#define FLAG_OF 0x0800 // overflow

extern struct txbe_cpu cpu;
extern uint8_t ram[EBIOS_RAM_SIZE];

void txbe_cpu_set_flags_sub16 (uint16_t, uint16_t, uint32_t);
void txbe_cpu_set_flags_sub8 (uint8_t, uint8_t, uint16_t);

void txbe_cpu_set_flags_inc8 (uint8_t, uint8_t);
void txbe_cpu_set_flags_inc16 (uint16_t, uint16_t);

#endif