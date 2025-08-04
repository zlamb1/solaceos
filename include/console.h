#ifndef CONSOLE_H
#define CONSOLE_H 1

#include <stdint.h>

#define ANSI_COLOR_BLACK          0
#define ANSI_COLOR_RED            1
#define ANSI_COLOR_GREEN          2
#define ANSI_COLOR_YELLOW         3
#define ANSI_COLOR_BLUE           4
#define ANSI_COLOR_MAGENTA        5
#define ANSI_COLOR_CYAN           6
#define ANSI_COLOR_WHITE          7
#define ANSI_COLOR_BRIGHT_BLACK   8
#define ANSI_COLOR_BRIGHT_RED     9
#define ANSI_COLOR_BRIGHT_GREEN   10
#define ANSI_COLOR_BRIGHT_YELLOW  11
#define ANSI_COLOR_BRIGHT_BLUE    12
#define ANSI_COLOR_BRIGHT_MAGENTA 13
#define ANSI_COLOR_BRIGHT_CYAN    14
#define ANSI_COLOR_BRIGHT_WHITE   15

typedef struct
{
  enum
  {
    CONSOLE_COLOR_TYPE_ANSI,
    CONSOLE_COLOR_TYPE_RGB,
  } type;
  union
  {
    uint8_t ansi;
    struct
    {
      uint8_t r, g, b;
    };
  };
} console_color_t;

typedef struct console_t
{
  uint32_t x, y, w, h;
  console_color_t fg, bg;
  int (*putch) (struct console_t *con, char ch);
  int (*setxy) (struct console_t *con, uint32_t x, uint32_t y);
  int (*setfg) (struct console_t *con, console_color_t fg);
  int (*setbg) (struct console_t *con, console_color_t bg);
  int (*scroll) (struct console_t *con);
  int (*clr) (struct console_t *con);
} console_t;

extern console_t *console;

console_t null_console_init (void);

void console_advance (console_t *con);
void console_newline (console_t *con);

int console_putchar (console_t *con, char ch);

#endif