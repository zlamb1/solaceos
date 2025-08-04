#include "loader/mmap.h"
#include "kprint.h"
#include "loader/boot.h"
#include "loader/fail.h"
#include "memblock.h"

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

static void
printstorage (uint64_t bytes)
{
  uint64_t kib = bytes / 1024;
  uint64_t mib = kib / 1024;
  uint64_t gib = mib / 1024;
  if (gib)
    {
      kprintf ("%4llu GiB", gib);
      return;
    }
  if (mib)
    {
      kprintf ("%4llu MiB", mib);
      return;
    }
  if (kib)
    {
      kprintf ("%4llu KiB", kib);
      return;
    }
  kprintf ("%4llu B  ", bytes);
}

static const char *
get_memblock_type_string (memblock_type_t type)
{
  switch (type)
    {
    case MEMBLOCK_TYPE_FREE:
      return "Free";
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
  while (addr < mb1_info->mmap_addr + mb1_info->mmap_size)
    {
      mmap_entry = (mb1_mmap_entry_t *) addr;
      if (mmap_entry->type == MB1_MMAP_ENTRY_TYPE_FREE
          && mmap_entry->base_addr + mmap_entry->length
                 > boot_info->max_physical_addr)
        boot_info->max_physical_addr
            = mmap_entry->base_addr + mmap_entry->length;
      memblock_reserve (mmap_entry->base_addr,
                        mmap_entry->base_addr + mmap_entry->length,
                        get_memblock_type_from_mb1 (mmap_entry->type));
      addr += mmap_entry->size + 4;
    }
  for (size_t i = 0; i < nmemblocks; i++)
    {
      memblock_t *memblock = memblocks + i;
      kprintf ("Memblock: [ ");
      printstorage (memblock->start);
      kprintf (" -> ");
      printstorage (memblock->end);
      kprintf (" Type=%-8s ]\n", get_memblock_type_string (memblock->type));
    }
}