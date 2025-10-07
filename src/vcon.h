#ifndef VCON_H
#define VCON_H 1

#include "common.h"
#include "font.h"
#include "limine.h"

#define VCON_NOGRAPHICS { .nographics = 1 }

struct vcon
{
  int nographics;
  u32 width;
  u32 height;
  u32 lpad;
  u32 tpad;
  u32 cx;
  u32 cy;
  usize pixel_stride;
  struct font font;
  struct limine_framebuffer fb;
};

struct vcon vcon_init (struct limine_framebuffer *fb);
void vcon_putchar (struct vcon *con, char c);
void vcon_write (struct vcon *con, const char *s);

#endif