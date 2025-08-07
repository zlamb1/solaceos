#include <stddef.h>
#include <stdint.h>

#include "kprint.h"
#include "memblock.h"
#include "palloc.h"
#include "panic.h"
#include "util.h"

typedef struct page_t
{
  struct page_t *next;
} page_t;

page_t *pages = NULL;

void
palloc_init (boot_info_t *bi)
{
  size_t nmemblocks, npages = 0;
  memblock_t *memblock;
  nmemblocks = bi->memblocks.size / sizeof (memblock_t);
  if (bi->memblocks.size % sizeof (memblock_t) != 0)
    kpanic ("palloc_init: invalid boot info");
  memblock = (memblock_t *) bi->memblocks.addr;
  for (size_t i = 0; i < nmemblocks; i++, memblock++)
    {
      uint64_t addr;
      if (memblock->type != MEMBLOCK_TYPE_FREE)
        continue;
      addr = ALIGN_UP (memblock->start, 4096);
      if (addr > memblock->end || memblock->end - addr < 4096)
        continue;
      while (addr < memblock->end)
        {
          free_page ((void *) (addr + VADDR));
          npages++;
          addr += 4096;
        }
    }
  kprintf ("Solace: Initialized %zu Pages\n", npages);
}

void *
alloc_page (void)
{
  page_t *page;
  if (pages == NULL)
    return NULL;
  page = pages;
  pages = page->next;
  return page;
}

void
free_page (void *p)
{
  page_t *page;
  if (((uintptr_t) p & 4095) != 0)
    kpanic ("free_page: misaligned pointer");
  page = (page_t *) p;
  page->next = pages;
  pages = page;
}