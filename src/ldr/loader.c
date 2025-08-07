#include "elf.h"
#include "kprint.h"
#include "loader/boot.h"
#include "loader/fail.h"
#include "loader/mmap.h"
#include "loader/paging.h"
#include "mb1.h"
#include "memblock.h"
#include "string.h"
#include "util.h"
#include "vga.h"

#define VADDR 0xffff800000000000

#define MB1_CMDLINE_OFFSET         0
#define MB1_MODS_OFFSET            1
#define MB1_MODS_CMDLINE_OFFSET    2
#define MB1_SYMS_OFFSET            3
#define MB1_MMAP_OFFSET            4
#define MB1_DRIVES_OFFSET          5
#define MB1_BOOTLOADER_NAME_OFFSET 6
#define MB1_APM_TABLE_OFFSET       7
#define MB1_VBE_OFFSET             8
#define MB1_FRAMEBUFFER_OFFSET     9
#define MB1_MAX_OFFSET             10

extern char _LDR_START;
extern char _LDR_END;

extern uint64_t gdt_start;
extern uint64_t gdtr_offset;

void __attribute__ ((noreturn)) lmain (mb1_info_t *mb1_info);

static vga_console_t vga;
static mb1_mod_t *kern_mod;
static boot_info_t boot_info;
static uint64_t krn_vaddr = UINT64_MAX, krn_vaddr_end = 0,
                krn_paddr = UINT64_MAX;

uint64_t bi_vaddr;

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

static uint32_t
setup_mb1_info (mb1_info_t *mb1_info, mb1_info_t *new_mb1_info,
                uint32_t bi_addr)
{
  if (new_mb1_info != NULL)
    {
      new_mb1_info->flags = mb1_info->flags;
      new_mb1_info->mem_lower = mb1_info->mem_lower;
      new_mb1_info->mem_upper = mb1_info->mem_upper;
      new_mb1_info->part3 = mb1_info->part3;
      new_mb1_info->part2 = mb1_info->part2;
      new_mb1_info->part1 = mb1_info->part1;
      new_mb1_info->drive = mb1_info->drive;
      new_mb1_info->vbe = mb1_info->vbe;
      new_mb1_info->framebuffer = mb1_info->framebuffer;
    }

  if (mb1_info->flags & MB1_INFO_FLAG_CMDLINE)
    {
      int len = strlen ((const char *) mb1_info->cmdline) + 1;
      if (new_mb1_info != NULL)
        {
          memcpy ((void *) bi_addr, (const void *) mb1_info->cmdline, len);
          new_mb1_info->cmdline = bi_addr;
        }
      bi_addr += len;
    }

  bi_addr = ALIGN_UP (bi_addr, _Alignof (mb1_mod_t));
  if (new_mb1_info != NULL)
    {
      memcpy ((void *) bi_addr, (const void *) mb1_info->mods_addr,
              sizeof (mb1_mod_t) * mb1_info->mods_count);
      new_mb1_info->mods_addr = bi_addr;
      new_mb1_info->mods_count = mb1_info->mods_count;
      bi_addr += sizeof (mb1_mod_t) * mb1_info->mods_count;
      for (size_t i = 0; i < new_mb1_info->mods_count; i++)
        {
          mb1_mod_t *mod = (mb1_mod_t *) new_mb1_info->mods_addr + i;
          int len = strlen ((const char *) mod->string) + 1;
          memcpy ((void *) bi_addr, (const void *) mod->string, len);
          mod->string = bi_addr;
          bi_addr += len;
        }
    }
  else
    {
      bi_addr += sizeof (mb1_mod_t) * mb1_info->mods_count;
      for (size_t i = 0; i < mb1_info->mods_count; i++)
        {
          mb1_mod_t *mod = (mb1_mod_t *) mb1_info->mods_addr + i;
          bi_addr += strlen ((const char *) mod->string) + 1;
        }
    }

  if (mb1_info->flags & MB1_INFO_FLAG_MMAP)
    {
      bi_addr = ALIGN_UP (bi_addr, _Alignof (mb1_mmap_entry_t));
      if (new_mb1_info != NULL)
        {
          memcpy ((void *) bi_addr, (const void *) mb1_info->mmap_addr,
                  mb1_info->mmap_length);
          new_mb1_info->mmap_addr = bi_addr;
          new_mb1_info->mmap_length = mb1_info->mmap_length;
        }
      bi_addr += mb1_info->mmap_length;
    }

  if (mb1_info->flags & MB1_INFO_FLAG_DRIVES)
    {
      bi_addr = ALIGN_UP (bi_addr, _Alignof (mb1_drive_entry_t));
      if (new_mb1_info != NULL)
        {
          memcpy ((void *) bi_addr, (const void *) mb1_info->drives_addr,
                  mb1_info->drives_length);
          new_mb1_info->drives_addr = bi_addr;
          new_mb1_info->drives_length = mb1_info->mmap_length;
        }
      bi_addr += mb1_info->drives_length;
    }

  if (mb1_info->flags & MB1_INFO_FLAG_BOOTLOADER_NAME)
    {
      int len = strlen ((const char *) mb1_info->boot_loader_name) + 1;
      if (new_mb1_info != NULL)
        {
          memcpy ((void *) bi_addr, (const void *) mb1_info->boot_loader_name,
                  len);
          new_mb1_info->boot_loader_name = bi_addr;
        }
      bi_addr += strlen ((const char *) mb1_info->boot_loader_name) + 1;
    }

  if (mb1_info->flags & MB1_INFO_FLAG_APM_TABLE)
    {
      bi_addr = ALIGN_UP (bi_addr, _Alignof (mb1_apm_table_t));
      if (new_mb1_info != NULL)
        {
          memcpy ((void *) bi_addr, (const void *) mb1_info->apm_table,
                  sizeof (mb1_apm_table_t));
          new_mb1_info->apm_table = bi_addr;
        }
      bi_addr += sizeof (mb1_apm_table_t);
    }

  if (mb1_info->flags & MB1_INFO_FLAG_FRAMEBUFFER
      && mb1_info->framebuffer.type == MB1_FRAMEBUFFER_TYPE_INDEXED)
    {
      bi_addr = ALIGN_UP (boot_info.size, _Alignof (mb1_color_descriptor_t));
      if (new_mb1_info != NULL)
        {
          memcpy ((void *) bi_addr,
                  (const void *) mb1_info->framebuffer.palette_addr,
                  sizeof (mb1_color_descriptor_t)
                      * mb1_info->framebuffer.palette_num_colors);
          new_mb1_info->framebuffer.palette_addr = bi_addr;
        }
      bi_addr += sizeof (mb1_color_descriptor_t)
                 * mb1_info->framebuffer.palette_num_colors;
    }

  return bi_addr;
}

static uint32_t
setup_boot_info (mb1_info_t *mb1_info)
{
  boot_info_t *new_bi;
  mb1_info_t *new_mb1_info;
  uint64_t tmp;
  uint32_t bi_addr, bi_off, mb1_off;

  boot_info.size = 0;

  get_page_tables_size (&boot_info, VADDR, krn_vaddr, krn_vaddr_end);

  bi_off = boot_info.size;

  boot_info.size += sizeof (boot_info_t);

  boot_info.size = ALIGN_UP (boot_info.size, _Alignof (mb1_info_t));
  mb1_off = boot_info.size;
  boot_info.size += sizeof (mb1_info_t);

  boot_info.size = setup_mb1_info (mb1_info, NULL, boot_info.size);

  if (krn_vaddr < VADDR + boot_info.size && krn_vaddr_end > VADDR)
    fail ("Boot Information Overlaps Kernel In Virtual Address Space");

  if (memblock_allocate (&tmp, boot_info.size, MEMBLOCK_ALLOC_FLAG_BELOW_4G))
    fail ("Failed To Allocate Space For Boot Info Structure");

  bi_addr = tmp;

  setup_page_tables (&boot_info, bi_addr, krn_paddr);

  new_bi = (boot_info_t *) (uintptr_t) (bi_addr + bi_off);
  new_bi->size = boot_info.size;
  new_bi->max_physical_addr = boot_info.max_physical_addr;
  new_bi->ldr.addr = (uintptr_t) &_LDR_START + VADDR;
  new_bi->ldr.size = (uintptr_t) &_LDR_END - (uintptr_t) &_LDR_START;
  new_bi->krn.addr = krn_vaddr;
  new_bi->krn.size = krn_vaddr_end - krn_vaddr;
  memcpy (new_bi->pt, boot_info.pt,
          sizeof (boot_page_table_t) * PAGE_TABLE_DEPTH);
  for (int i = 0; i < PAGE_TABLE_DEPTH; i++)
    new_bi->pt[i].pt_seg.addr += bi_addr + VADDR;
  new_bi->mode = boot_info.mode;
  new_bi->mb1_info.addr = bi_addr + mb1_off + VADDR;
  new_bi->mb1_info.size = boot_info.size - mb1_off;

  new_mb1_info = (mb1_info_t *) (bi_addr + mb1_off);

  setup_mb1_info (mb1_info, new_mb1_info,
                  bi_addr + mb1_off + sizeof (mb1_info_t));

  bi_vaddr = VADDR + bi_addr + bi_off;

  return bi_addr + boot_info.ipt[3].pt_seg.addr;
}

static uint64_t
load_kernel (void)
{
  const char *msg;
  uint64_t krn_mod_size;
  elf_hdr_64_t *hdr = (elf_hdr_64_t *) kern_mod->start;
  int is_entry_inside_segment = 0;

  if (kern_mod->start >= kern_mod->end)
    fail ("Bootloader Passed Malformed Kernel Module");

  krn_mod_size = kern_mod->end - kern_mod->start;

  /* TODO: abstract this to arch layer */

  if (krn_mod_size < sizeof (elf_hdr_64_t))
    fail ("Kernel Module Smaller Than ELF Header");
  if (elf_arch_validate_hdr (hdr, &msg))
    fail ("%s", msg);

  if (krn_mod_size < hdr->e_phoff + sizeof (elf_ph_64_t))
    fail ("Kernel Module Smaller Than ELF Program "
          "Header");

  if (kern_mod->start + hdr->e_phoff + hdr->e_phentsize * hdr->e_phnum
      >= kern_mod->end)
    fail ("Program Header Truncated");

  if (!hdr->e_phnum)
    fail ("Kernel Image Has No Segments");

  for (int i = 0; i < hdr->e_phnum; i++)
    {
      elf_ph_64_t *ph
          = (elf_ph_64_t *) (uintptr_t) (kern_mod->start + hdr->e_phoff
                                         + hdr->e_phentsize * i);
      if (ph->p_type != PT_LOAD || !ph->p_memsz)
        continue;

      if (ph->p_offset + ph->p_filesz >= kern_mod->end)
        fail ("Solace32: Boot Failed: Invalid Program Header Entry\nSegment "
              "Out Of Bounds Of File");

      if (ph->p_filesz > ph->p_memsz)
        fail ("Invalid Program Header Entry\nFile Size "
              "> Memory Size");

      if (ph->p_vaddr < (uintptr_t) &_LDR_END
          && (uintptr_t) &_LDR_START < ph->p_vaddr + ph->p_memsz)
        fail ("Kernel Overlaps Loader");

      if (ph->p_paddr + ph->p_memsz > 0xFFFFFFFF)
        fail ("Kernel Requested Physical Address Outside 4GiB");

      if (ph->p_vaddr + ph->p_memsz < ph->p_vaddr)
        fail ("Kernel Virtual Address Too High To Fit Kernel Size");

      if (memblock_reserve (ph->p_paddr, ph->p_paddr + ph->p_memsz,
                            MEMBLOCK_TYPE_KERNEL, 0)
          != 0)
        fail ("Failed To Reserve Kernel Memory");

      memcpy ((void *) (uintptr_t) ph->p_paddr,
              (void *) (uintptr_t) (kern_mod->start + ph->p_offset),
              ph->p_filesz);

      if (ph->p_memsz - ph->p_filesz)
        memset ((void *) (uintptr_t) (ph->p_paddr + ph->p_filesz), 0,
                ph->p_memsz - ph->p_filesz);

      if (ph->p_flags & PF_X && hdr->e_entry >= ph->p_vaddr
          && hdr->e_entry < ph->p_vaddr + ph->p_memsz)
        is_entry_inside_segment = 1;

      if (ph->p_vaddr < krn_vaddr)
        krn_vaddr = ph->p_vaddr;

      if (ph->p_vaddr + ph->p_memsz > krn_vaddr_end)
        krn_vaddr_end = ph->p_vaddr + ph->p_memsz;

      if (ph->p_paddr < krn_paddr)
        krn_paddr = ph->p_paddr;
    }

  if (!is_entry_inside_segment)
    fail ("Kernel Entry Point Outside Loadable Segments");

  return hdr->e_entry;
}

void
lmain (mb1_info_t *mb1_info)
{
  storage_unit_t max_physical_addr_su;
  uint64_t krn_entry;
  uint32_t pml4t_addr;

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
    fail ("Failed to Locate Kernel Module");
  mmap (&boot_info, mb1_info);

  max_physical_addr_su = get_storage_unit (boot_info.max_physical_addr);
  kprintf ("Solace32: Max Physical Address: %#llx [ %llu %s ]\n",
           boot_info.max_physical_addr, max_physical_addr_su.value,
           max_physical_addr_su.name);

  memblock_reserve ((uint64_t) (uintptr_t) &_LDR_START,
                    (uint64_t) (uintptr_t) &_LDR_END, MEMBLOCK_TYPE_ALLOCATED,
                    0);
  memblock_reserve (kern_mod->start, kern_mod->end, MEMBLOCK_TYPE_ALLOCATED,
                    0);

  reserve_mb1_info (mb1_info);

  krn_entry = load_kernel ();

  pml4t_addr = setup_boot_info (mb1_info);

  print_mmap ();

  kprintf ("Solace32: Entering Kernel\n");
  enter_kernel (pml4t_addr, krn_entry, bi_vaddr);
}