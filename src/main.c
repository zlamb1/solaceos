#include "mb1.h"

void __attribute__ ((noreturn)) kmain (mb1_info_t *info);

void
kmain (mb1_info_t *mb1_info)
{
  char *vmem = (char *) 0xB8000;
  (void) mb1_info;
  vmem[1] = 0xFF;
  for (;;)
    ;
}