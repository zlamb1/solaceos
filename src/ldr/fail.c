#include "ldr/fail.h"
#include "kprint.h"

void
fail (const char *fmt, ...)
{
  va_list args;
  va_start (args, fmt);
  kprintf ("Solace32: ");
  kprintf ("Boot Failed: ");
  kvprintf (fmt, args);
  kprintf ("\n");
  va_end (args);
  /* TODO: provide timeout */
  for (;;)
    ;
}