#include "mb1.h"

void __attribute__ ((noreturn)) lmain (mb1_info_t *mb1_info);

static int line = 1;

static void
printline (const char *str)
{
  uint8_t *vmem = (uint8_t *) 0xB8000;
  vmem += 160 * line;
  while (str[0] != '\0')
    *vmem++ = *str++;
  line++;
}

void
lmain (mb1_info_t *mb1_info)
{
  (void) mb1_info;
  printline ("Solace32: Loader Entry");
  for (;;)
    ;
}