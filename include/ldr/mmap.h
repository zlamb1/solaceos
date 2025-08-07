#ifndef MMAP_H
#define MMAP_H

#include "boot.h"
#include "mb1.h"

typedef struct
{
  const char *name;
  uint64_t value;
} storage_unit_t;

storage_unit_t get_storage_unit (uint64_t bytes);
void mmap (boot_info_t *boot_info, mb1_info_t *mb1_info);
void print_mmap (void);

void reserve_mb1_info (mb1_info_t *mb1_info);

#endif