#include "emu.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

#include "cpu.h"
#include "event.h"
#include "int10.h"
#include "thread.h"
#include "x86_16b.h"

static txbe_thread emu_thread_id;
static enum txbe_emu_state emu_state = EMU_STOPPED;

void txbe_emu_backend_start() {
  struct txbe_event e;
  e.type = EVENT_START;
  txbe_event_push(&e);
}

void txbe_emu_backend_end() {
  struct txbe_event e;
  e.type = EVENT_END;
  txbe_event_push(&e);
}

void txbe_emu_start() {
  emu_state = EMU_RUNNING;
  txbe_thread_create(&emu_thread_id, txbe_emu_thread, NULL);
}

void txbe_emu_pause() {
  emu_state = EMU_PAUSED;
}

void txbe_emu_resume() {
  emu_state = EMU_RUNNING;
}

void txbe_emu_stop() {
  emu_state = EMU_STOPPED;
  txbe_thread_join(emu_thread_id);
}

THREAD txbe_emu_thread(void* _udata) {
  txbe_emu_backend_start();
  while (emu_state != EMU_STOPPED) {
    if (emu_state == EMU_RUNNING) {
      uint16_t physical_ip = (cpu.cs << 4) + cpu.ip;
      txbe_emu_emulate_next(physical_ip);
    } else {
      txbe_thread_sleep(1);
    }
  }
  txbe_emu_backend_end();
  return THREAD_RET_NULL;
}

void txbe_emu_emulate() {
  txbe_emu_start();
  while (1) {
    txbe_backend_event_process_queue();
  }
  txbe_emu_stop();
}

void txbe_emu_handle_interrupt(uint8_t int_num) {
  if (int_num == 0x10) {
    txbe_int10h_handle();
  }
  // soon iwill add more
}

// calculate 16-bit address from ModR/M
uint16_t txbe_emu_calc_rm16(uint8_t rm) {
  switch (rm & 7) {
    case 0: return cpu.bx + cpu.si;
    case 1: return cpu.bx + cpu.di;
    case 2: return cpu.bp + cpu.si;
    case 3: return cpu.bp + cpu.di;
    case 4: return cpu.si;
    case 5: return cpu.di;
    case 6: return cpu.bp;
    case 7: return cpu.bx;
  }
  return 0;
}

int txbe_emu_emulate_next(uint16_t physical_ip) {
  uint8_t opcode = ram[physical_ip];

  uint16_t* reg16[] = { &cpu.ax, &cpu.cx, &cpu.dx, &cpu.bx, &cpu.sp, &cpu.bp, &cpu.si, &cpu.di };
  uint8_t* reg8_low[] = { (uint8_t*)&cpu.ax, (uint8_t*)&cpu.cx, (uint8_t*)&cpu.dx, (uint8_t*)&cpu.bx };
  uint8_t* reg8_high[] = { ((uint8_t*)&cpu.ax)+1, ((uint8_t*)&cpu.cx)+1, ((uint8_t*)&cpu.dx)+1, ((uint8_t*)&cpu.bx)+1 };

  switch (opcode) {
    case 0x00: // NOP placeholder
      cpu.ip++;
      break;

    /** =============== CMP ================== */
    // CMP r/m8, r8
    case 0x38: {
      uint8_t modrm = ram[physical_ip + 1];
      uint8_t reg = (modrm >> 3) & 7;
      uint8_t rm  = modrm & 7;
      uint8_t op1, op2;
      if ((modrm >> 6) == 3) {
        // reg-reg
        op1 = *reg8_low[rm];
        op2 = *reg8_low[reg];
      } else {
        // mem-reg
        uint16_t addr = txbe_emu_calc_rm16(rm);
        op1 = ram[addr];
        op2 = *reg8_low[reg];
      }
      uint16_t res = (uint16_t)op1 - (uint16_t)op2;
      txbe_cpu_set_flags_sub8(op1, op2, res);
      cpu.ip += 2;
      break;
    }

    // CMP r/m16, r16
    case 0x39: {
      uint8_t modrm = ram[physical_ip + 1];
      uint8_t reg = (modrm >> 3) & 7;
      uint8_t rm  = modrm & 7;
      uint16_t op1, op2;
      if ((modrm >> 6) == 3) {
        op1 = *reg16[rm];
        op2 = *reg16[reg];
      } else {
        uint16_t addr = txbe_emu_calc_rm16(rm);
        op1 = ram[addr] | (ram[addr+1] << 8);
        op2 = *reg16[reg];
      }
      uint32_t res = (uint32_t)op1 - (uint32_t)op2;
      txbe_cpu_set_flags_sub16(op1, op2, res);
      cpu.ip += 2;
      break;
    }

    // CMP r8, r/m8
    case 0x3A: {
      uint8_t modrm = ram[physical_ip + 1];
      uint8_t reg = (modrm >> 3) & 7;
      uint8_t rm  = modrm & 7;
      uint8_t op1, op2;
      if ((modrm >> 6) == 3) {
        op1 = *reg8_low[reg];
        op2 = *reg8_low[rm];
      } else {
        uint16_t addr = txbe_emu_calc_rm16(rm);
        op1 = *reg8_low[reg];
        op2 = ram[addr];
      }
      uint16_t res = (uint16_t)op1 - (uint16_t)op2;
      txbe_cpu_set_flags_sub8(op1, op2, res);
      cpu.ip += 2;
      break;
    }

    // CMP r16, r/m16
    case 0x3B: {
      uint8_t modrm = ram[physical_ip + 1];
      uint8_t reg = (modrm >> 3) & 7;
      uint8_t rm  = modrm & 7;
      uint16_t op1, op2;
      if ((modrm >> 6) == 3) {
        op1 = *reg16[reg];
        op2 = *reg16[rm];
      } else {
        uint16_t addr = txbe_emu_calc_rm16(rm);
        op1 = *reg16[reg];
        op2 = ram[addr] | (ram[addr+1] << 8);
      }
      uint32_t res = (uint32_t)op1 - (uint32_t)op2;
      txbe_cpu_set_flags_sub16(op1, op2, res);
      cpu.ip += 2;
      break;
    }

    // CMP AL, imm8
    case 0x3C: {
      uint8_t imm8 = ram[physical_ip + 1];
      uint8_t al = (uint8_t)cpu.ax;
      uint16_t res = (uint16_t)al - (uint16_t)imm8;
      txbe_cpu_set_flags_sub8(al, imm8, res);
      cpu.ip += 2;
      break;
    }

    // CMP AX, imm16
    case 0x3D: {
      uint16_t imm16 = ram[physical_ip + 1] | (ram[physical_ip + 2] << 8);
      uint16_t ax = cpu.ax;
      uint32_t res = (uint32_t)ax - (uint32_t)imm16;
      txbe_cpu_set_flags_sub16(ax, imm16, res);
      cpu.ip += 3;
      break;
    }
    /** =============== CMP ================== */

    /** =============== JMP ================== */
    case 0xEB: { // JMP rel8
      int8_t rel8 = (int8_t)ram[physical_ip + 1];
      cpu.ip = (uint16_t)((int16_t)cpu.ip + 2 + rel8);
      break;
    }

    case 0x74: { // JE rel8
      int8_t rel8 = (int8_t)ram[physical_ip + 1];
      if (cpu.flags & FLAG_ZF)
        cpu.ip = (uint16_t)((int16_t)cpu.ip + 2 + rel8);
      else
        cpu.ip += 2;
      break;
    }
    /** =============== JMP ================== */

    /** =============== INT ================== */
    case 0xCD: { // INT imm8
      uint8_t int_num = ram[physical_ip + 1];
      txbe_emu_handle_interrupt(int_num);
      cpu.ip += 2;
      break;
    }
    /** =============== INT ================== */

    /** =============== INC ================== */
    // 8 bit
    case 0x40: { // INC AL
        uint8_t old = txbe_x86_16_getal();
        cpu.ax = (cpu.ax & 0xFF00) | (old + 1);
        txbe_cpu_set_flags_inc8(old, old + 1);
        cpu.ip++;
        break;
    }
    case 0x44: { // INC AH
        uint8_t old = txbe_x86_16_getah();
        cpu.ax = (cpu.ax & 0x00FF) | ((old + 1) << 8);
        txbe_cpu_set_flags_inc8(old, old + 1);
        cpu.ip++;
        break;
    }
    case 0x41: { // INC CL
        uint8_t old = txbe_x86_16_getcl();
        cpu.cx = (cpu.cx & 0xFF00) | (old + 1);
        txbe_cpu_set_flags_inc8(old, old + 1);
        cpu.ip++;
        break;
    }
    case 0x45: { // INC CH
        uint8_t old = txbe_x86_16_getch();
        cpu.cx = (cpu.cx & 0x00FF) | ((old + 1) << 8);
        txbe_cpu_set_flags_inc8(old, old + 1);
        cpu.ip++;
        break;
    }
    case 0x42: { // INC DL
        uint8_t old = txbe_x86_16_getdl();
        cpu.dx = (cpu.dx & 0xFF00) | (old + 1);
        txbe_cpu_set_flags_inc8(old, old + 1);
        cpu.ip++;
        break;
    }
    case 0x46: { // INC DH
        uint8_t old = txbe_x86_16_getdh();
        cpu.dx = (cpu.dx & 0x00FF) | ((old + 1) << 8);
        txbe_cpu_set_flags_inc8(old, old + 1);
        cpu.ip++;
        break;
    }
    case 0x43: { // INC BL
        uint8_t old = txbe_x86_16_getbl();
        cpu.bx = (cpu.bx & 0xFF00) | (old + 1);
        txbe_cpu_set_flags_inc8(old, old + 1);
        cpu.ip++;
        break;
    }
    case 0x47: { // INC BH
        uint8_t old = txbe_x86_16_getbh();
        cpu.bx = (cpu.bx & 0x00FF) | ((old + 1) << 8);
        txbe_cpu_set_flags_inc8(old, old + 1);
        cpu.ip++;
        break;
    }
    
    // INC 16-bit
    case 0x48: cpu.ax++; txbe_cpu_set_flags_inc16(cpu.ax - 1, cpu.ax); break; // INC AX
    case 0x49: cpu.cx++; txbe_cpu_set_flags_inc16(cpu.cx - 1, cpu.cx); break; // INC CX
    case 0x4A: cpu.dx++; txbe_cpu_set_flags_inc16(cpu.dx - 1, cpu.dx); break; // INC DX
    case 0x4B: cpu.bx++; txbe_cpu_set_flags_inc16(cpu.bx - 1, cpu.bx); break; // INC BX
    case 0x4C: cpu.sp++; txbe_cpu_set_flags_inc16(cpu.sp - 1, cpu.sp); break; // INC SP
    case 0x4D: cpu.bp++; txbe_cpu_set_flags_inc16(cpu.bp - 1, cpu.bp); break; // INC BP
    case 0x4E: cpu.si++; txbe_cpu_set_flags_inc16(cpu.si - 1, cpu.si); break; // INC SI
    case 0x4F: cpu.di++; txbe_cpu_set_flags_inc16(cpu.di - 1, cpu.di); break; // INC DI
    /** =============== INC ================== */

    /** =============== MOV ================== */
    // MOV immediate 8-bit: B0..B7
    case 0xB0: case 0xB1: case 0xB2: case 0xB3:
    case 0xB4: case 0xB5: case 0xB6: case 0xB7: {
      uint8_t imm8 = ram[physical_ip + 1];
      if (opcode <= 0xB3)
        *reg8_low[opcode - 0xB0] = imm8;
      else
        *reg8_high[opcode - 0xB4] = imm8;
      cpu.ip += 2;
      break;
    }

    // MOV immediate 16-bit: B8..BF
    case 0xB8: case 0xB9: case 0xBA: case 0xBB:
    case 0xBC: case 0xBD: case 0xBE: case 0xBF: {
      uint16_t imm16 = ram[physical_ip + 1] | (ram[physical_ip + 2] << 8);
      *reg16[opcode - 0xB8] = imm16;
      cpu.ip += 3;
      break;
    }

    // MOV r/m8, r8
    case 0x88: {
      uint8_t modrm = ram[physical_ip + 1];
      uint8_t reg = (modrm >> 3) & 7;
      uint8_t rm  = modrm & 7;
      if ((modrm >> 6) == 3) {
        // reg to reg
        *reg8_low[rm] = *reg8_low[reg];
      } else {
        // reg to mem
        uint16_t addr = txbe_emu_calc_rm16(rm);
        ram[addr] = *reg8_low[reg];
      }
      cpu.ip += 2;
      break;
    }

    // MOV r/m16, r16
    case 0x89: {
      uint8_t modrm = ram[physical_ip + 1];
      uint8_t reg = (modrm >> 3) & 7;
      uint8_t rm  = modrm & 7;
      if ((modrm >> 6) == 3) {
        // reg to reg
        *reg16[rm] = *reg16[reg];
      } else {
        // reg to mem
        uint16_t addr = txbe_emu_calc_rm16(rm);
        ram[addr] = *reg16[reg] & 0xFF;
        ram[addr+1] = (*reg16[reg] >> 8) & 0xFF;
      }
      cpu.ip += 2;
      break;
    }

    // MOV r8, r/m8
    case 0x8A: {
      uint8_t modrm = ram[physical_ip + 1];
      uint8_t reg = (modrm >> 3) & 7;
      uint8_t rm  = modrm & 7;
      if ((modrm >> 6) == 3) {
        // reg to reg
        *reg8_low[reg] = *reg8_low[rm];
      } else {
        // mem to reg
        uint16_t addr = txbe_emu_calc_rm16(rm);
        *reg8_low[reg] = ram[addr];
      }
      cpu.ip += 2;
      break;
    }

    // MOV r16, r/m16
    case 0x8B: {
      uint8_t modrm = ram[physical_ip + 1];
      uint8_t reg = (modrm >> 3) & 7;
      uint8_t rm  = modrm & 7;
      if ((modrm >> 6) == 3) {
        // reg to reg
        *reg16[reg] = *reg16[rm];
      } else {
        // mem to reg
        uint16_t addr = txbe_emu_calc_rm16(rm);
        *reg16[reg] = ram[addr] | (ram[addr+1] << 8);
      }
      cpu.ip += 2;
      break;
    }
    /** =============== MOV ================== */

    default: {
      struct txbe_event e;
      e.type = EVENT_UNKNOWN_OPCODE;
      e.unknown_opcode.opcode = opcode;
      txbe_event_push(&e);
      txbe_emu_pause();
      break;
    }
  }

  return 0;
}