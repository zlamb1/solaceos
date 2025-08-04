#ifndef MEMBLOCK_H
#define MEMBLOCK_H 1

#include <stddef.h>
#include <stdint.h>

typedef enum
{
  MEMBLOCK_TYPE_FREE = 0,
  MEMBLOCK_TYPE_ALLOCATED = 1,
  MEMBLOCK_TYPE_ACPI = 2,
  MEMBLOCK_TYPE_RESERVED = 3,
  MEMBLOCK_TYPE_BAD_RAM = 4,
  MEMBLOCK_TYPE_MAX = 5
} memblock_type_t;

#define MEMBLOCK_FLAG_BELOW_4G (1 << 0)

typedef struct
{
  /* [start, end) */
  uint64_t start, end;
  unsigned char type;
} memblock_t;

extern size_t nmemblocks;
extern memblock_t memblocks[];

int memblock_has_capacity (void);
int memblock_append (uint64_t start, uint64_t end, memblock_type_t type);

int memblock_reserve (uint64_t start, uint64_t end, memblock_type_t type);
int memblock_allocate (uint64_t *start, uint64_t size, int flags);

#endif