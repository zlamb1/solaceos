#include "kprint.h"
#include "print.h"

#define PREALLOC 128

static console_t null_console;

console_t *console;

void
initprint (console_t *con)
{
  if (con == NULL)
    {
      null_console = null_console_init ();
      con = &null_console;
    }
  else
    {
      console = con;
      con->clr (con);
    }
}

int
kvprintf (const char *fmt, va_list args)
{
  int len;
  char tmp[PREALLOC];
  len = kvsnprintf (tmp, PREALLOC, fmt, args);
  if (len > PREALLOC)
    len = PREALLOC - 1;
  for (int i = 0; i < len; i++)
    console_putchar (console, tmp[i]);
  return len;
}

int
kprintf (const char *fmt, ...)
{
  int len;
  va_list args;
  va_start (args, fmt);
  len = kvprintf (fmt, args);
  va_end (args);
  return len;
}