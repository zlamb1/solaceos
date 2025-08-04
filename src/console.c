#include <stddef.h>

#include "console.h"
#include "errno.h"

static int
null_console_putch (console_t *con, char ch)
{
  (void) con;
  (void) ch;
  return -ENOSYS;
}

static int
null_console_setxy (console_t *con, uint32_t x, uint32_t y)
{
  (void) con;
  (void) x;
  (void) y;
  return -ENOSYS;
}

static int
null_console_setfg (console_t *con, console_color_t fg)
{
  (void) con;
  (void) fg;
  return -ENOSYS;
}

static int
null_console_setbg (console_t *con, console_color_t bg)
{
  (void) con;
  (void) bg;
  return -ENOSYS;
}

static int
null_console_scroll (console_t *con)
{
  (void) con;
  return -ENOSYS;
}

static int
null_console_clr (console_t *con)
{
  (void) con;
  return -ENOSYS;
}

console_t
null_console_init (void)
{
  return (console_t){ .x = 0,
                      .y = 0,
                      .w = 0,
                      .h = 0,
                      .putch = null_console_putch,
                      .setxy = null_console_setxy,
                      .setfg = null_console_setfg,
                      .setbg = null_console_setbg,
                      .scroll = null_console_scroll,
                      .clr = null_console_clr };
}

void
console_advance (console_t *con)
{
  uint32_t x = con->x, y = con->y;
  if (++x >= con->w)
    {
      x = 0;
      console_newline (con);
    }
  else
    con->setxy (con, x, y);
}

void
console_newline (console_t *con)
{
  uint32_t x = con->x, y = con->y;
  if (++y >= con->h)
    {
      y = con->h - 1;
      con->scroll (con);
    }
  con->setxy (con, x, y);
}

int
console_putchar (console_t *con, char ch)
{
  int result;
  switch (ch)
    {
    case '\b':
      if (con->x)
        con->setxy (con, con->x - 1, con->y);
      else if (con->y)
        con->setxy (con, con->w - 1, con->y - 1);
      break;
    case '\n':
      con->setxy (con, 0, con->y);
      // fallthrough
    case '\r':
      console_newline (con);
      break;
    default:
      if ((result = con->putch (con, ch)))
        return result;
      console_advance (con);
      break;
    }
  return 0;
}