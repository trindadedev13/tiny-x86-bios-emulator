#include "boot.h"

#include <stdio.h>
#include <stdlib.h>

#include "cpu.h"

int txbe_boot_load_bootloader(const char* name) {
  FILE* boot = fopen(name, "rb");
  if (!boot) {
    printf("Failed to open boot file.\n");
    return -1;
  }

  size_t bytes_read =
      fread(&ram[BOOTLOADER_MEM_ADDR], 1, BOOTLOADER_SIZE, boot);
  fclose(boot);

  if (bytes_read < BOOTLOADER_SIZE) {
    printf("Your bootloader should be at first 512 bytes of image.\n");
    return -1;
  }

  uint16_t* sig = (uint16_t*)&ram[BOOTLOADER_MEM_ADDR + BOOTLOADER_EXEC_SIZE];
  if (*sig != 0xAA55) {
    printf(
        "Bootloader signature missing, Expected 0xAA55 At Bootloader end.\n");
    return -1;
  }

  // set sign to 0 cause we dont need it when emulating
  *sig = 0x0000;

  cpu.cs = 0x07C0;
  cpu.ip = 0x0000;
  return 0;
}