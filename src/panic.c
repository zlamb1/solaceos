#include <limits.h>

#include "common.h"
#include "limine/limine.h"
#include "panic.h"

static struct limine_framebuffer *fb = NULL;

#define NUM_GLYPHS 256
#define GLYPH_SIZE 8

_Static_assert (UCHAR_MAX < NUM_GLYPHS,
                "char encoding does not fit glyphs array");

void
kpanic_init (struct limine_framebuffer *_fb)
{
  fb = _fb;
}

static always_inline void
set_pixel_no_fmt (u32 x, u32 y, u8 white, usize pixel_stride)
{
  usize off = y * fb->pitch + x * pixel_stride;
  u8 *data;

  if ((off + 3) > fb->height * fb->pitch)
    for (;;)
      ;

  data = (u8 *) fb->address + off;

  data[0] = white ? 0xFF : 0;
  data[1] = white ? 0xFF : 0;
  data[2] = white ? 0xFF : 0;
}

static always_inline void
set_pixel (u32 x, u32 y, u8 r, u8 g, u8 b, usize pixel_stride)
{
  usize off = y * fb->pitch + x * pixel_stride;
  u8 *data;

  if ((off + 3) > fb->height * fb->pitch)
    for (;;)
      ;

  data = (u8 *) fb->address + off;

#define COMP(S, V)                                                            \
  switch (S)                                                                  \
    {                                                                         \
    case 0:                                                                   \
      data[0] = (V);                                                          \
      break;                                                                  \
    case 8:                                                                   \
      data[1] = (V);                                                          \
      break;                                                                  \
    case 16:                                                                  \
      data[2] = (V);                                                          \
      break;                                                                  \
    default:                                                                  \
      for (;;)                                                                \
        ;                                                                     \
    }

  COMP (fb->red_mask_shift, r);
  COMP (fb->green_mask_shift, g);
  COMP (fb->blue_mask_shift, b);

#undef COMP

  return;
}

void
kpanic (const char *msg)
{
  usize pixel_stride;
  u8 supported_fmt = 1;

  (void) msg;

  if (fb == NULL)
    goto loop;

  switch (fb->bpp)
    {
    case 24:
      pixel_stride = 3;
      break;
    case 32:
      pixel_stride = 4;
      break;
    default:
      goto loop;
    }

#define COMP(S)                                                               \
  switch (S)                                                                  \
    {                                                                         \
    case 0:                                                                   \
    case 8:                                                                   \
    case 16:                                                                  \
      break;                                                                  \
    default:                                                                  \
      supported_fmt = 0;                                                      \
      goto fill_screen;                                                       \
    }

  COMP (fb->red_mask_shift);
  COMP (fb->green_mask_shift);
  COMP (fb->blue_mask_shift);

fill_screen:
  for (u32 y = 0; y < fb->height; ++y)
    for (u32 x = 0; x < fb->width; ++x)
      supported_fmt ? set_pixel (x, y, 0, 0, 255, pixel_stride)
                    : set_pixel_no_fmt (x, y, 0, pixel_stride);

loop:
  for (;;)
    ;
}