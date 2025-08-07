#include "console.h"
#include "ldr/boot.h"
#include "mb1.h"
#include "vga.h"

vga_console_t vga;

console_t *
get_static_console (boot_mode_t boot_mode)
{
  mb1_info_t *mb1_info;
  if (boot_mode.type != BOOT_MODE_TYPE_MB1)
    return NULL;
  mb1_info = boot_mode.mb1_info;
  if (!(mb1_info->flags & MB1_INFO_FLAG_FRAMEBUFFER)
      || mb1_info->framebuffer.type != MB1_FRAMEBUFFER_TYPE_TEXT)
    return NULL;
  vga = vga_console_init ((void *) (mb1_info->framebuffer.addr + VADDR),
                          mb1_info->framebuffer.width,
                          mb1_info->framebuffer.height,
                          mb1_info->framebuffer.pitch);
  return &vga.base;
}