#include "print.h"
#include "vcon.h"

static struct vcon con;

void
kprint_init (struct limine_framebuffer_response *fb_response)
{
  struct limine_framebuffer *fb = NULL;

  for (uint64_t i = 0; i < fb_response->framebuffer_count; i++)
    {
      struct limine_framebuffer *_fb = fb_response->framebuffers[i];

      if (_fb->memory_model != LIMINE_FRAMEBUFFER_RGB)
        continue;

      fb = _fb;
      break;
    }

  if (!fb)
    return;

  con = vcon_init (fb);
}

void
kprint (const char *fmt)
{
  vcon_write (&con, fmt);
}

void
kprintln (const char *fmt)
{
  vcon_write (&con, fmt);
  vcon_putchar (&con, '\n');
}