#ifndef BOOT_H
#define BOOT_H

#include <stdint.h>

#ifndef VADDR
#define VADDR 0xffff800000000000
#endif

#ifndef PAGE_TABLE_DEPTH
#define PAGE_TABLE_DEPTH 4
#endif

typedef enum
{
  BOOT_MODE_MB1
} boot_mode_t;

typedef struct
{
  uint64_t size; /* size in bytes */
  uint64_t addr; /* virtual address */
} boot_segment_t;

typedef struct
{
  boot_segment_t pt_seg;
  uint64_t pt_nument;
  uint16_t pt_off;
  uint16_t pad[3];
} boot_page_table_t;

typedef struct
{
  uint64_t max_physical_addr;             /* max addressable physical memory */
  boot_segment_t bi;                      /* location of boot information */
  boot_segment_t ldr;                     /* location of loader */
  boot_segment_t krn;                     /* location of kernel */
  boot_page_table_t pt[PAGE_TABLE_DEPTH]; /* physical mappings */
  boot_page_table_t ipt[PAGE_TABLE_DEPTH]; /* identity mappings */
  boot_page_table_t kpt[PAGE_TABLE_DEPTH]; /* kernel mappings */
  boot_segment_t memblocks;
  uint64_t mode;
  union
  {
    boot_segment_t mb1_info; /* location of Multiboot1 info and data */
  };
} boot_info_t;

void __attribute__ ((noreturn))
enter_kernel (uint32_t pml4t_addr, uint64_t krn_entry, uint64_t bi_vaddr);

#endif