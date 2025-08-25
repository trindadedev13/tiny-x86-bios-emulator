# Tiny x86 Bios Emulator
  txbe is a Tiny (Bios) x86 16-bit Emulator

# Instructions Implemented
  NOP               (0x00 and 0x90)  
  CMP r/m8, r8      (38 /r)  
  CMP r/m16, r16    (39 /r)  
  CMP r8, r/m8      (3A /r)  
  CMP r16, r/m16    (3B /r)  
  CMP AL, imm8      (3C ib)  
  CMP AX, imm16     (3D iw)  
  JMP rel8          (EB cb)  
  JE rel8           (74 cb)  
  INT imm8          (CD ib)  
  INC 8-bit  
    INC AL          (0xC0)  
    INC CL          (0xC1)  
    INC DL          (0xC2)  
    INC BL          (0xC3)  
    INC AH          (0xC4)  
    INC CH          (0xC5)  
    INC DH          (0xC6)  
    INC BH          (0xC7)  
  INC 16-bit  
    INC AX          (0x40)  
    INC CX          (0x41)  
    INC DX          (0x42)  
    INC BX          (0x43)  
    INC SP          (0x44)  
    INC BP          (0x45)  
    INC SI          (0x46)  
    INC DI          (0x47)  
  MOV r8, imm8      (B0–B7)  
  MOV r16, imm16    (B8–BF)  
  MOV r/m8, r8      (88 /r)  
  MOV r/m16, r16    (89 /r)  
  MOV r8, r/m8      (8A /r)  
  MOV r16, r/m16    (8B /r)  

# Infrastructure
  The tbxe project is separated into 2 main parts, Core and Backend.  
## Core:
  Core is what loads the bootloader, interprets the x86 bytecodes  
  Core produces events for each instruction type, you can see: [event.h](https://github.com/trindadedev13/txbe/tree/main/event.hL6)  

## Backend:
  Backend consumes the events that were pushed by the Core  
  Backend is responsible for implementing interruptions  

## About Backends
  Currently we have just one (1) backend, the stdout backend for Terminal Directly Output.  
  You can implement your own backend with [backend-template](https://github.com/trindadedev13/txbe/tree/main/backend_template.c)  
  And add it in CMakeLists.txt  

# Building Ebios
  To build Ebios you have some ways  
  But before compiling, we should take a look at dependencies

## Dependencies
  CMake & Make  
  Any C Compiler compatible to CMake  
  NASM to compile the Bootloader (for testing)

Well Now we can take a look at how we can compile  
There's two way to compile it:

## Ruby Buildscript
  If you have ruby installed, you can just run:  
  ```bash
  ruby build.rb --build
  ```  
  To run:  
  ```bash
  ruby build.rb --run
  ```  
  If everything goes well, so you'll see 'boot' in stdout.

  To run with gdb:
  ```bash
  ruby build.rb --gdb --run # add --buuld too for debug build.
  ```  

## Manually CMake
  Well, if you don't want use the ruby script so you have to do it manually  
  To build:
  ```bash
  cmake -B build -S . -DBACKEND=stdout
  cmake --build build
  ```  
  Above you can see i used `-DBACKEND=stdout`, it's for indicates to CMake what backend we want use.  
  You can see all backends [here](https://github.com/trindadedev13/txbe/README.md#available-backends).  

  To Run:
  Well, to run the compiled executable, we just need to execute de elf (or exe in windows) passing an Bootloader .img/.iso  
  You can use the our SimpleBootloader.asm for testing  
  We use nasm to compile it:
  ```bash
  nasm -f bin SimpleBootloader.asm -o build/SimpleBootloader.img
  # Now we grant de permission
  chmod +x ./build/txbe_e
  # Now we can finally run it passing the SimpleBootloader.img
  ./build/txbe_e build/SimpleBootloader.img
  ```  
  If everything goes well, so you'll see 'boot' in stdout.