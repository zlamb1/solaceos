#include <stddef.h>
#include <string.h>

#include "arch/x86_64/io.h"
#include "console.h"
#include "errno.h"
#include "util.h"
#include "vga.h"

#define COLOR(fg, bg) (fg & 0xF) | (bg << 4)

#define MISC_OUTPUT_READ_REG     0x3CC
#define MISC_OUTPUT_WRITE_REG    0x3C2
#define CRTC_ADDRESS_REG         0x3D4
#define CRTC_DATA_REG            0x3D5
#define CRTC_MAX_SCANLINE_REG    0x009
#define CRTC_CURSOR_START_REG    0x00A
#define CRTC_CURSOR_END_REG      0x00B
#define CRTC_CURSOR_HIGH_LOC_REG 0x00E
#define CRTC_CURSOR_LOW_LOC_REG  0x00F

uint8_t ansi_to_vga_text_mode_0[16] = {
  [ANSI_COLOR_BLACK] = 0x0,        [ANSI_COLOR_RED] = 0x4,
  [ANSI_COLOR_GREEN] = 0x2,        [ANSI_COLOR_YELLOW] = 0x6,
  [ANSI_COLOR_BLUE] = 0x1,         [ANSI_COLOR_MAGENTA] = 0x5,
  [ANSI_COLOR_CYAN] = 0x3,         [ANSI_COLOR_WHITE] = 0x7,
  [ANSI_COLOR_BRIGHT_BLACK] = 0x8, [ANSI_COLOR_BRIGHT_RED] = 0xC,
  [ANSI_COLOR_BRIGHT_GREEN] = 0xA, [ANSI_COLOR_BRIGHT_YELLOW] = 0xE,
  [ANSI_COLOR_BRIGHT_BLUE] = 0x9,  [ANSI_COLOR_BRIGHT_MAGENTA] = 0xD,
  [ANSI_COLOR_BRIGHT_CYAN] = 0xB,  [ANSI_COLOR_BRIGHT_WHITE] = 0xF,
};

int
vga_putch (console_t *con, char ch)
{
  vga_console_t *vga;
  uint8_t *vmem;
  if (con == NULL)
    return -EINVAL;
  vga = container_of (con, vga_console_t, base);
  vmem = (uint8_t *) (vga->fb + con->y * (con->w << 1) + (con->x << 1));
  vmem[0] = ch;
  vmem[1] = vga->color;
  return 0;
}

int
vga_setxy (console_t *con, uint32_t x, uint32_t y)
{
  uint32_t pos;
  if (con == NULL || x >= con->w || y >= con->h)
    return -EINVAL;
  con->x = x;
  con->y = y;
  pos = y * 80 + x;
  outb (CRTC_ADDRESS_REG, CRTC_CURSOR_LOW_LOC_REG);
  outb (CRTC_DATA_REG, pos & 0xFF);
  outb (CRTC_ADDRESS_REG, CRTC_CURSOR_HIGH_LOC_REG);
  outb (CRTC_DATA_REG, (pos >> 8) & 0xFF);
  return 0;
}

int
vga_setfg (console_t *con, console_color_t fg)
{
  vga_console_t *vga;
  if (con == NULL)
    return -EINVAL;
  if (fg.type != CONSOLE_COLOR_TYPE_ANSI)
    return -ENOTSUP;
  if (fg.ansi > 0xF)
    return -EINVAL;
  con->fg = fg;
  vga = container_of (con, vga_console_t, base);
  vga->color = COLOR (fg.ansi, con->bg.ansi);
  return 0;
}

int
vga_setbg (console_t *con, console_color_t bg)
{
  vga_console_t *vga;
  if (con == NULL)
    return -EINVAL;
  if (bg.type != CONSOLE_COLOR_TYPE_ANSI)
    return -ENOTSUP;
  if (bg.ansi > 0xF)
    return -EINVAL;
  con->bg = bg;
  vga = container_of (con, vga_console_t, base);
  vga->color = COLOR (con->fg.ansi, bg.ansi);
  return 0;
}

int
vga_scroll (console_t *con)
{
  vga_console_t *vga;
  size_t bytes_per_row;
  char *src, *dst;
  if (con == NULL)
    return -EINVAL;
  vga = container_of (con, vga_console_t, base);
  bytes_per_row = con->w << 1;
  src = vga->fb + vga->pitch;
  dst = vga->fb;
  for (uint32_t y = 1; y < con->h; y++, src += vga->pitch, dst += vga->pitch)
    memcpy (dst, src, bytes_per_row);
  dst = vga->fb + (con->h - 1) * vga->pitch;
  for (uint32_t x = 0; x < con->w; x++, dst += 2)
    {
      dst[0] = ' ';
      dst[1] = vga->color;
    }
  return 0;
}

int
vga_clr (console_t *con)
{
  vga_console_t *vga;
  uint32_t y;
  char *vmem;
  vga = container_of (con, vga_console_t, base);
  con->setxy (con, 0, 0);
  for (y = 0, vmem = vga->fb; y < con->h; y++, vmem += vga->pitch)
    for (uint32_t x = 0; x < con->w; x++)
      {
        uint32_t idx = x << 1;
        vmem[idx] = ' ';
        vmem[idx + 1] = 0xF;
      }
  return 0;
}

vga_console_t
vga_console_init (void *fb, uint32_t w, uint32_t h, uint32_t pitch)
{
  console_color_t fg
      = { .type = CONSOLE_COLOR_TYPE_ANSI, .ansi = ANSI_COLOR_BRIGHT_WHITE };
  console_color_t bg
      = { .type = CONSOLE_COLOR_TYPE_ANSI, .ansi = ANSI_COLOR_BLACK };
  vga_console_t vga = { .base = { .w = w,
                                  .h = h,
                                  .fg = fg,
                                  .bg = bg,
                                  .putch = vga_putch,
                                  .setxy = vga_setxy,
                                  .setfg = vga_setfg,
                                  .setbg = vga_setbg,
                                  .scroll = vga_scroll,
                                  .clr = vga_clr },
                        .color = COLOR (fg.ansi, bg.ansi),
                        .fb = fb,
                        .pitch = pitch };
  return vga;
}