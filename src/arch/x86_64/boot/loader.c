#include "kprint.h"
#include "mb1.h"
#include "string.h"
#include "vga.h"

#define ROWS 80

void __attribute__ ((noreturn)) lmain (mb1_info_t *mb1_info);

static vga_console_t vga;

static mb1_mod_t *kern_mod;
static uint64_t max_addr = 0;

static void
fail (void)
{
  /* TODO: allow timeout */
  for (;;)
    ;
}

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

static void
iterate_mmap (mb1_info_t *mb1_info)
{
  uintptr_t addr = mb1_info->mmap_addr;
  mb1_mmap_entry_t *mmap_entry;
  if (!(mb1_info->flags & MB1_INFO_FLAG_MMAP))
    {
      kprintf ("Solace32: Boot Failed: No Memory Map Available\n");
      fail ();
    }
  if (mb1_info->mmap_addr < 4)
    {
      kprintf ("Solace32: Boot Failed: Multiboot1 Invalid Memory Map\n");
      fail ();
    }
  while (addr < mb1_info->mmap_addr + mb1_info->mmap_size)
    {
      mmap_entry = (mb1_mmap_entry_t *) addr;
      if (mmap_entry->type == MB1_MMAP_ENTRY_TYPE_FREE
          && mmap_entry->base_addr + mmap_entry->length > max_addr)
        max_addr = mmap_entry->base_addr + mmap_entry->length;
      addr += mmap_entry->size + 4;
    }
}

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
  kprintf ("Solace32: Loader Entry\n");
  if ((kern_mod = find_kernel_module (mb1_info)) == NULL)
    {
      kprintf ("Solace32: Boot Failed: Failed to Locate x64 Kernel Module\n");
      fail ();
    }
  iterate_mmap (mb1_info);
  kprintf ("Max Address: %llu GiB\n", max_addr / (1024 * 1024 * 1024));
  for (;;)
    ;
}