#include <stddef.h>

#include "kprint.h"
#include "print.h"
#include "spinlock.h"

#define PREALLOC 128

spinlock_t console_lock = { 0 };
static console_t null_console;

console_t *console;

const char prefix[] = "SolaceOS> ";

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
  int len = 0, tmplen, write_prefix = 0;
  char tmp[PREALLOC];
  console_color_t oldfg, newfg;

  switch ((signed char) fmt[0])
    {
    case -1:
      newfg = (console_color_t) {
        .type = CONSOLE_COLOR_TYPE_ANSI,
        .ansi = ANSI_COLOR_BRIGHT_RED,
      };
      goto should_write_prefix;
    case -63:
      newfg = (console_color_t) {
        .type = CONSOLE_COLOR_TYPE_ANSI,
        .ansi = ANSI_COLOR_BRIGHT_YELLOW,
      };
      goto should_write_prefix;
    case -64:
      newfg = (console_color_t) {
        .type = CONSOLE_COLOR_TYPE_ANSI,
        .ansi = ANSI_COLOR_BRIGHT_GREEN,
      };
    should_write_prefix:
      write_prefix = 1;
      fmt++;
      break;
    }

  tmplen = kvsnprintf (tmp, PREALLOC, fmt, args);
  if (tmplen > PREALLOC)
    tmplen = PREALLOC - 1;

  acquire_spinlock (&console_lock);
  oldfg = console->fg;

  if (write_prefix)
    {
      console->setfg (console, newfg);
      for (size_t i = 0; i < sizeof (prefix) - 1; i++)
        console_putchar (console, prefix[i]);
      console->setfg (console, oldfg);
      len += sizeof (prefix) - 1;
    }

  len += tmplen;
  for (int i = 0; i < tmplen; i++)
    console_putchar (console, tmp[i]);

  release_spinlock (&console_lock);

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