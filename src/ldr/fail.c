#include "loader/fail.h"
#include "kprint.h"

void
fail (const char *fmt, ...)
{
  va_list args;
  va_start (args, fmt);
  kvprintf (fmt, args);
  va_end (args);
  /* TODO: provide timeout */
  for (;;)
    ;
}