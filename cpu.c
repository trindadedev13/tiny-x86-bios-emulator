#include "cpu.h"

#include <stdint.h>

struct txbe_cpu cpu = {};
uint8_t ram[EBIOS_RAM_SIZE] = {};

// SUB/CMP 16-bit
void txbe_cpu_set_flags_sub16(uint16_t op1, uint16_t op2, uint32_t res) {
  cpu.flags &= ~(FLAG_CF | FLAG_PF | FLAG_AF | FLAG_ZF | FLAG_SF | FLAG_OF);

  // handle flags
  // carry
  if (res & 0x10000)
    cpu.flags |= FLAG_CF;

  // zero
  if ((res & 0xFFFF) == 0)
    cpu.flags |= FLAG_ZF;

  // sign
  if (res & 0x8000)
    cpu.flags |= FLAG_SF;

  // auxiliary carry (bit 4 borrow)
  if (((op1 ^ op2 ^ res) & 0x10))
    cpu.flags |= FLAG_AF;

  // overflow
  if (((op1 ^ op2) & (op1 ^ res) & 0x8000))
    cpu.flags |= FLAG_OF;

  // parity flag (result in 8 bits have parity pair)
  uint8_t r = res & 0xFF;
  r ^= r >> 4;
  r ^= r >> 2;
  r ^= r >> 1;
  if (!(r & 1))
    cpu.flags |= FLAG_PF;
}

// SUB/CMP 8-bit
void txbe_cpu_set_flags_sub8(uint8_t op1, uint8_t op2, uint16_t res) {
  cpu.flags &= ~(FLAG_CF | FLAG_PF | FLAG_AF | FLAG_ZF | FLAG_SF | FLAG_OF);

  // handle flags
  // carry
  if (res & 0x100)
    cpu.flags |= FLAG_CF;

  // zero
  if ((res & 0xFF) == 0)
    cpu.flags |= FLAG_ZF;

  // sign
  if (res & 0x80)
    cpu.flags |= FLAG_SF;

  // auxiliary carry
  if (((op1 ^ op2 ^ res) & 0x10))
    cpu.flags |= FLAG_AF;

  // overflow flag
  if (((op1 ^ op2) & (op1 ^ res) & 0x80))
    cpu.flags |= FLAG_OF;

  // parity flag
  uint8_t r = res & 0xFF;
  r ^= r >> 4;
  r ^= r >> 2;
  r ^= r >> 1;
  if (!(r & 1))
    cpu.flags |= FLAG_PF;
}

void txbe_cpu_set_flags_inc8(uint8_t old, uint8_t result) {
  cpu.flags &= ~(FLAG_SF | FLAG_ZF | FLAG_PF | FLAG_AF | FLAG_OF);

  if (result == 0) cpu.flags |= FLAG_ZF;
  if (result & 0x80) cpu.flags |= FLAG_SF;
  if (__builtin_parity(result) == 0) cpu.flags |= FLAG_PF; // paridade par
  if ((old & 0x0F) + 1 > 0x0F) cpu.flags |= FLAG_AF; // carry do nibble inferior
  if (old == 0x7F) cpu.flags |= FLAG_OF; // overflow
}

void txbe_cpu_set_flags_inc16(uint16_t old, uint16_t result) {
  cpu.flags &= ~(FLAG_SF | FLAG_ZF | FLAG_PF | FLAG_AF | FLAG_OF);

  if (result == 0) cpu.flags |= FLAG_ZF;
  if (result & 0x8000) cpu.flags |= FLAG_SF;
  if (__builtin_parity(result & 0xFF) == 0) cpu.flags |= FLAG_PF; // paridade do byte inferior
  if ((old & 0x0F) + 1 > 0x0F) cpu.flags |= FLAG_AF;
  if (old == 0x7FFF) cpu.flags |= FLAG_OF;
}