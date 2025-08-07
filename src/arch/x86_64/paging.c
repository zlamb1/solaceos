#include "ldr/paging.h"
#include "ldr/boot.h"
#include "ldr/fail.h"
#include "string.h"
#include "util.h"

extern char _LDR_END;

static void
_get_page_tables_size (boot_info_t *bi, boot_page_table_t *bpts,
                       uint64_t paddr_size, uint64_t vaddr,
                       uint64_t root_pt_addr, int align)
{
  for (int i = 0; i < PAGE_TABLE_DEPTH; i++)
    (bpts + i)->pt_off = (vaddr >> (12 + i * 9)) & 511;

  for (int i = 0; i < PAGE_TABLE_DEPTH; i++)
    {
      boot_page_table_t *pt = bpts + i;
      if (i)
        {
          boot_page_table_t *prev = bpts + i - 1;
          pt->pt_nument = ALIGN_UP (prev->pt_nument + prev->pt_off, 512) >> 9;
        }
      else
        pt->pt_nument
            = (align ? ALIGN_UP (paddr_size, 4096) : paddr_size) >> 12;
      if (i == PAGE_TABLE_DEPTH - 1)
        {
          if (pt->pt_nument + pt->pt_off > 512)
            fail ("Physical Address Mapping Overflowed Virtual Address Space");
          pt->pt_seg.addr = root_pt_addr;
          pt->pt_seg.size = 4096;
          continue;
        }
      pt->pt_seg.addr = bi->size;
      pt->pt_seg.size = ALIGN_UP (pt->pt_nument + pt->pt_off, 512) << 3;
      bi->size += pt->pt_seg.size;
    }
}

void
get_page_tables_size (boot_info_t *bi, uint64_t p_vaddr, uint64_t krn_vaddr,
                      uint64_t krn_vaddr_end)
{
  uint64_t root_pt_addr;

  if (p_vaddr & 4095)
    fail ("Virtual Address Must Be Page Aligned");

  root_pt_addr = bi->size;
  bi->size += 4096;

  /* TODO: check if any of these virtual address ranges step on each other */
  _get_page_tables_size (bi, &bi->pt[0], bi->max_physical_addr, p_vaddr,
                         root_pt_addr, 0);
  _get_page_tables_size (bi, &bi->ipt[0], (uintptr_t) &_LDR_END, 0,
                         root_pt_addr, 1);
  _get_page_tables_size (bi, &bi->kpt[0], krn_vaddr_end - krn_vaddr, krn_vaddr,
                         root_pt_addr, 1);
}

static void
_setup_page_tables (boot_page_table_t *bpts, uintptr_t bi_addr, uint64_t paddr)
{
  for (int i = 0; i < PAGE_TABLE_DEPTH; i++)
    {
      boot_page_table_t *pt = bpts + i;
      uint64_t prev = (i ? bpts[i - 1].pt_seg.addr + bi_addr : paddr), diff,
               *p = (uint64_t *) (uintptr_t) (pt->pt_seg.addr + bi_addr);
      if (pt->pt_off && i < PAGE_TABLE_DEPTH - 1)
        memset (p, 0, sizeof (uint64_t) * pt->pt_off);
      for (uint64_t j = 0; j < pt->pt_nument; j++)
        p[j + pt->pt_off] = (prev + (j << 12)) | 3;
      diff = ALIGN_UP (pt->pt_nument + pt->pt_off, 512)
             - (pt->pt_nument + pt->pt_off);
      if (diff && i < PAGE_TABLE_DEPTH - 1)
        memset (p + pt->pt_nument + pt->pt_off, 0, sizeof (uint64_t) * diff);
    }
}

void
setup_page_tables (boot_info_t *bi, uintptr_t bi_addr, uintptr_t krn_paddr)
{
  boot_page_table_t *root_pt = bi->pt + (PAGE_TABLE_DEPTH - 1);
  memset ((uint8_t *) (uintptr_t) (bi_addr + root_pt->pt_seg.addr), 0,
          root_pt->pt_seg.size);
  _setup_page_tables (bi->pt, bi_addr, 0);
  _setup_page_tables (bi->ipt, bi_addr, 0);
  _setup_page_tables (bi->kpt, bi_addr, krn_paddr);
}