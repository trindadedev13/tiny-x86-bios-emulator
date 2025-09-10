#include <stdio.h>

#include "boot.h"
#include "emu.h"

int
main (int argc, char **argv)
{
  if (argc < 2)
    {
      printf ("Please provide a boot .img/.iso\n");
      return 1;
    }
  const char *file = argv[1];
  txbe_boot_load_bootloader (file);
  txbe_emu_emulate ();
}