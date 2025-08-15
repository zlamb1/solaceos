#include <stddef.h>

#include "kprint.h"
#include "print.h"
#include "spinlock.h"

#define PREALLOC       128
#define INIT_KLOG_SIZE 1024

static console_t null_console;
static char init_kern_log_buf[INIT_KLOG_SIZE];
static const char prefix[] = "SolaceOS> ";

kern_log_t kern_log = { .log = init_kern_log_buf, .lock = SPINLOCK_INIT };

void
initprint (console_t *con)
{
  if (con == NULL)
    {
      null_console = null_console_init ();
      kern_log.console = &null_console;
    }
  else
    {
      kern_log.console = con;
      con->clr (con);
    }
}

int
kvprintf (const char *fmt, va_list args)
{
  lockflags_t flags;
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

  flags = spin_lock_irqsave (&kern_log.lock);
  oldfg = kern_log.console->fg;

  if (write_prefix)
    {
      kern_log.console->setfg (kern_log.console, newfg);
      console_write (kern_log.console, sizeof (prefix) - 1, prefix);
      kern_log.console->setfg (kern_log.console, oldfg);
      len += sizeof (prefix) - 1;
    }

  len += tmplen;
  console_write (kern_log.console, tmplen, tmp);

  spin_unlock_irqrestore (&kern_log.lock, flags);

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