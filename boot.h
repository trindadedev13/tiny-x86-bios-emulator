#ifndef __txbe_boot_h__
#define __txbe_boot_h__

#include <stdint.h>

/** bootloader start at memory ram */
#define BOOTLOADER_MEM_ADDR 0x7C00

/** bootloader file size (bytes) */
#define BOOTLOADER_SIZE 512

/** executable size of bootloader */
/** BOOTLOADER_SIZE - sign 2-bytes */
#define BOOTLOADER_EXEC_SIZE 510

int txbe_boot_load_bootloader (const char *);

#endif