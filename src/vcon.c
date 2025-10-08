#include "vcon.h"
#include "common.h"
#include "fence.h"
#include "limine/limine.h"
#include "mem.h"
#include "psf.h"

extern char _binary_consolefont_psf_start;
extern char _binary_consolefont_psf_end;

struct vcon
vcon_init (struct limine_framebuffer *fb)
{
  struct vcon con;

  if ((uintptr_t) &_binary_consolefont_psf_end
      <= (uintptr_t) &_binary_consolefont_psf_start)
    goto nographics;

  usize psf_size = (uintptr_t) &_binary_consolefont_psf_end
                   - (uintptr_t) &_binary_consolefont_psf_start;

  if (psf_parse (&con.font, (void *) &_binary_consolefont_psf_start, psf_size)
      != PSF_SUCCESS)
    goto nographics;

  if (con.font.glyph_width > fb->width || con.font.glyph_height > fb->height)
    goto nographics;

  if (fb->memory_model != LIMINE_FRAMEBUFFER_RGB
      || (fb->bpp != 24 && fb->bpp != 32)
      || (fb->red_mask_size != 8 || fb->green_mask_size != 8
          || fb->blue_mask_size != 8))
    goto nographics;

  kmemcpy (&con.fb, fb, sizeof (struct limine_framebuffer));

  con.nographics   = 0;
  con.width        = fb->width / con.font.glyph_width;
  con.height       = fb->height / con.font.glyph_height;
  con.lpad         = (fb->width % con.font.glyph_width) / 2;
  con.tpad         = (fb->height % con.font.glyph_height) / 2;
  con.cx           = 0;
  con.cy           = 0;
  con.pixel_stride = fb->bpp >> 3;

  return con;

nographics:
  return (struct vcon) VCON_NOGRAPHICS;
}

static always_inline u32
get_real_x (struct vcon *con, u32 cx)
{
  return cx * con->font.glyph_width + con->lpad;
}

static always_inline u32
get_real_y (struct vcon *con, u32 cy)
{
  return cy * con->font.glyph_height + con->tpad;
}

static void
vcon_putc (struct vcon *con, char c, u32 cx, u32 cy)
{
  u8 *fb           = con->fb.address;
  u8 *glyph        = con->font.glyphs + con->font.glyph_stride * (u8) c;
  usize row_stride = (con->font.glyph_width + 7) >> 3;

  for (u32 y = 0; y < con->font.glyph_height; y++)
    {
      for (u32 x = 0; x < con->font.glyph_width; x++)
        {
          u8 byte     = glyph[y * row_stride + (x >> 3)];
          usize index = (get_real_y (con, cy) + y) * con->fb.pitch
                        + (get_real_x (con, cx) + x) * con->pixel_stride;

          if (!((byte << (x % 8)) & 0x80))
            {
              fb[index]     = 0;
              fb[index + 1] = 0;
              fb[index + 2] = 0;
              continue;
            }

          fb[index]     = 0xFF;
          fb[index + 1] = 0xFF;
          fb[index + 2] = 0xFF;
        }
    }
}

static void
vcon_scroll (struct vcon *con)
{
  if (con->nographics || !con->width || !con->height)
    return;

  usize y;
  usize glyph_row_stride = con->font.glyph_height * con->fb.pitch;

  u8 *fb      = con->fb.address;
  u8 *currow  = fb + get_real_y (con, 0) * con->fb.pitch;
  u8 *nextrow = currow + glyph_row_stride;

  for (y = 0; y < con->height - 1;
       ++y, currow = nextrow, nextrow += glyph_row_stride)
    kmemcpy (currow, nextrow, glyph_row_stride);

  kmemset (currow, 0, glyph_row_stride);
}

static always_inline void
vcon_advance (struct vcon *con)
{
  con->cx = 0;
  if (++con->cy == con->height)
    {
      con->cy = con->height - 1;
      vcon_scroll (con);
    }
}

void
vcon_putchar (struct vcon *con, char c)
{
  if (con->nographics || !con->width || !con->height)
    return;

  switch (c)
    {
    case '\b':
      if (con->cx)
        {
          --con->cx;
          vcon_putchar (con, ' ');
          --con->cx;
        }
      break;
    case '\r':
      con->cx = 0;
      break;
    case '\n':
      vcon_advance (con);
      break;
    default:
      vcon_putc (con, c, con->cx++, con->cy);

      if (con->cx == con->width)
        vcon_advance (con);

      break;
    }
}

void
vcon_write (struct vcon *con, const char *s)
{
  for (; s[0]; ++s)
    vcon_putchar (con, s[0]);
}

void
vcon_sync (struct vcon *con)
{
  if (con->nographics)
    return;

  sfence ();
}