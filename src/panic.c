#include "panic.h"
#include "kprint.h"

void
kpanic (const char *msg)
{
  kprintf (KPRINT_ERR "Panic: %s", msg);
  for (;;)
    ;
}