#include "panic.h"
#include "kprint.h"

void
kpanic (const char *msg)
{
  kprintf ("Solace: Panic: %s\n", msg);
  for (;;)
    ;
}