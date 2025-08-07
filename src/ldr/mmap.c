#include "ldr/mmap.h"
#include "kprint.h"
#include "ldr/boot.h"
#include "ldr/fail.h"
#include "mb1.h"
#include "memblock.h"
#include "string.h"

static memblock_type_t
get_memblock_type_from_mb1 (uint8_t type)
{
  switch (type)
    {
    case MB1_MMAP_ENTRY_TYPE_FREE:
      return MEMBLOCK_TYPE_FREE;
    case MB1_MMAP_ENTRY_TYPE_ACPI:
      return MEMBLOCK_TYPE_ACPI;
    case MB1_MMAP_ENTRY_TYPE_RESERVED:
      return MEMBLOCK_TYPE_RESERVED;
    case MB1_MMAP_ENTRY_TYPE_BAD_RAM:
      return MEMBLOCK_TYPE_BAD_RAM;
    default:
      return MEMBLOCK_TYPE_RESERVED;
    }
}

storage_unit_t
get_storage_unit (uint64_t bytes)
{
  uint64_t kib = bytes / 1024;
  uint64_t mib = kib / 1024;
  uint64_t gib = mib / 1024;
  if (gib >= 10)
    return (storage_unit_t){ .name = "GiB", .value = gib };
  if (mib >= 10)
    return (storage_unit_t){ .name = "MiB", .value = mib };
  if (kib)
    return (storage_unit_t){ .name = "KiB", .value = kib };
  return (storage_unit_t){ .name = "B  ", .value = bytes };
}

static const char *
get_memblock_type_string (memblock_type_t type)
{
  switch (type)
    {
    case MEMBLOCK_TYPE_FREE:
      return "Free";
    case MEMBLOCK_TYPE_KERNEL:
      return "Kernel";
    case MEMBLOCK_TYPE_ALLOCATED:
      return "Allocated";
    case MEMBLOCK_TYPE_ACPI:
      return "ACPI";
    case MEMBLOCK_TYPE_BAD_RAM:
      return "Bad Ram";
    default:
      return "Reserved";
    }
}

void
mmap (boot_info_t *boot_info, mb1_info_t *mb1_info)
{
  uintptr_t addr = mb1_info->mmap_addr;
  mb1_mmap_entry_t *mmap_entry;
  if (!(mb1_info->flags & MB1_INFO_FLAG_MMAP))
    fail ("Solace32: Boot Failed: No Memory Map Available\n");
  if (mb1_info->mmap_addr < 4)
    fail ("Solace32: Boot Failed: Multiboot1 Invalid Memory Map\n");
  while (addr < mb1_info->mmap_addr + mb1_info->mmap_length)
    {
      mmap_entry = (mb1_mmap_entry_t *) addr;
      if (mmap_entry->type == MB1_MMAP_ENTRY_TYPE_FREE
          && mmap_entry->base_addr + mmap_entry->length
                 > boot_info->max_physical_addr)
        boot_info->max_physical_addr
            = mmap_entry->base_addr + mmap_entry->length;
      memblock_reserve (mmap_entry->base_addr,
                        mmap_entry->base_addr + mmap_entry->length,
                        get_memblock_type_from_mb1 (mmap_entry->type), 0);
      addr += mmap_entry->size + 4;
    }
}

void
print_mmap (void)
{
  memblock_t *memblock = NULL;

  while ((memblock = memblocks_iterate (memblock)) != NULL)
    {
      storage_unit_t start = get_storage_unit (memblock->start),
                     end = get_storage_unit (memblock->end);
      kprintf ("Solace32: Memblock: [ %4llu %s -> %4llu %s, Type=%-9s ]\n",
               start.value, start.name, end.value, end.name,
               get_memblock_type_string (memblock->type));
    }
}

void
reserve_mb1_info (mb1_info_t *mb1_info)
{
#define TRY_ALLOC(START, END)                                                 \
  do                                                                          \
    {                                                                         \
      if (memblock_reserve ((START), (END), MEMBLOCK_TYPE_ALLOCATED, 0) < 0)  \
        fail ("Solace32: Boot Failed: Failed to Reserve Multiboot 1 "         \
              "Information\n");                                               \
    }                                                                         \
  while (0)
  if (mb1_info->flags & MB1_INFO_FLAG_CMDLINE)
    TRY_ALLOC (mb1_info->cmdline,
               mb1_info->cmdline + strlen ((const char *) mb1_info->cmdline)
                   + 1);
  TRY_ALLOC (mb1_info->mods_addr,
             mb1_info->mods_addr + sizeof (mb1_info_t) * mb1_info->mods_count);
  for (size_t i = 0; i < mb1_info->mods_count; i++)
    {
      mb1_mod_t *mod = (mb1_mod_t *) mb1_info->mods_addr + i;
      TRY_ALLOC (mod->string,
                 mod->string + strlen ((const char *) mod->string) + 1);
    }
  if (mb1_info->flags & MB1_INFO_FLAG_MMAP)
    TRY_ALLOC (mb1_info->mmap_addr,
               mb1_info->mmap_addr + mb1_info->mmap_length);
  if (mb1_info->flags & MB1_INFO_FLAG_DRIVES)
    TRY_ALLOC (mb1_info->drives_addr,
               mb1_info->drives_addr + mb1_info->drives_length);
  if (mb1_info->flags & MB1_INFO_FLAG_BOOTLOADER_NAME)
    TRY_ALLOC (mb1_info->boot_loader_name,
               mb1_info->boot_loader_name
                   + strlen ((const char *) mb1_info->boot_loader_name) + 1);
  if (mb1_info->flags & MB1_INFO_FLAG_APM_TABLE)
    TRY_ALLOC (mb1_info->apm_table,
               mb1_info->apm_table + sizeof (mb1_apm_table_t));
  if (mb1_info->flags & MB1_INFO_FLAG_FRAMEBUFFER
      && mb1_info->framebuffer.type == MB1_FRAMEBUFFER_TYPE_INDEXED)
    TRY_ALLOC (mb1_info->framebuffer.palette_addr,
               mb1_info->framebuffer.palette_addr
                   + sizeof (mb1_color_descriptor_t)
                         * mb1_info->framebuffer.palette_num_colors);
#undef TRY_ALLOC
}