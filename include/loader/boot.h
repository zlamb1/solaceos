#ifndef BOOT_H
#define BOOT_H

#include <stdint.h>

typedef struct
{
  uint64_t size; /* size in bytes */
  uint64_t addr; /* virtual address */
} boot_segment_t;

typedef struct
{
  uint64_t size;              /* total size of boot info */
  uint64_t max_physical_addr; /* max addressable physical memory */
  boot_segment_t ldr;         /* location of loader */
  boot_segment_t krn;         /* location of kernel */
  boot_segment_t pml4;        /* location of PML4 table  */
  boot_segment_t pdpt;        /* location of page directory pointer tables */
  boot_segment_t pdt;         /* location of page directory tables */
  boot_segment_t pt;          /* location of page tables */
  enum
  {
    BOOT_MODE_MB1
  } mode;
  union
  {
    boot_segment_t mb1_info; /* location of Multiboot1 info and data */
  };
} boot_info_t;

#endif