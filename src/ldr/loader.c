#include "kprint.h"
#include "loader/boot.h"
#include "loader/fail.h"
#include "loader/mmap.h"
#include "mb1.h"
#include "string.h"
#include "vga.h"

void __attribute__ ((noreturn)) lmain (mb1_info_t *mb1_info);

static vga_console_t vga;

static mb1_mod_t *kern_mod;

static boot_info_t boot_info;

static mb1_mod_t *
find_kernel_module (mb1_info_t *mb1_info)
{
  uint32_t i;
  mb1_mod_t *mod;
  if (!(mb1_info->flags & MB1_INFO_FLAG_MODS))
    return NULL;
  for (i = 0, mod = (mb1_mod_t *) mb1_info->mods_addr;
       i < mb1_info->mods_count; i++, mod++)
    {
      if (strcmp ((const char *) mod->string, "kern64") == 0)
        return mod;
    }
  return NULL;
}

/*static void
get_reserved_space (void)
{
  uint64_t reserved_space = 0, npages, mod;
  npages = boot_info.max_physical_addr / 4096;
  if (boot_info.max_physical_addr % 4096)
    ++npages;
  for (int i = 0; i < 4; i++)
    {
      mod = npages % 512;
      npages >>= 9;
      if (mod)
        ++npages;
      reserved_space += npages << 12;
    }
}*/

void
lmain (mb1_info_t *mb1_info)
{
  if (!(mb1_info->flags & MB1_INFO_FLAG_FRAMEBUFFER)
      || mb1_info->framebuffer.type != MB1_FRAMEBUFFER_TYPE_TEXT)
    initprint (NULL);
  else
    {
      mb1_framebuffer_t *mb1_fb = &mb1_info->framebuffer;
      vga = vga_console_init ((void *) (uintptr_t) mb1_fb->addr, mb1_fb->width,
                              mb1_fb->height, mb1_fb->pitch);
      initprint (&vga.base);
    }
  kprintf ("Solace32: Entering Loader\n");
  if ((kern_mod = find_kernel_module (mb1_info)) == NULL)
    {
      fail ("Solace32: Boot Failed: Failed to Locate x64 Kernel Module\n");
    }
  mmap (&boot_info, mb1_info);
  // get_reserved_space ();
  kprintf ("Solace32: Entering Kernel\n");
  for (;;)
    ;
}