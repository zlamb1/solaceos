#ifndef VGA_H
#define VGA_H 1

#include "console.h"

typedef struct
{
  uint8_t color;
  uint32_t pitch;
  char *fb;
  console_t base;
} vga_console_t;

int vga_putch (console_t *con, char ch);
int vga_setxy (console_t *con, uint32_t x, uint32_t y);
int vga_setfg (console_t *con, console_color_t fg);
int vga_setbg (console_t *con, console_color_t bg);
int vga_scroll (console_t *con);
int vga_clr (console_t *con);

vga_console_t vga_console_init (void *fb, uint32_t w, uint32_t h,
                                uint32_t pitch);

#endif
