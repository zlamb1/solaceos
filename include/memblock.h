#ifndef MEMBLOCK_H
#define MEMBLOCK_H 1

#include <stddef.h>
#include <stdint.h>

typedef enum
{
  MEMBLOCK_TYPE_FREE = 0,
  MEMBLOCK_TYPE_KERNEL = 1,
  MEMBLOCK_TYPE_ALLOCATED = 2,
  MEMBLOCK_TYPE_ACPI = 3,
  MEMBLOCK_TYPE_RESERVED = 4,
  MEMBLOCK_TYPE_BAD_RAM = 5,
  MEMBLOCK_TYPE_MAX = 6
} memblock_type_t;

#define MEMBLOCK_RESERVE_FLAG_FORCE (1 << 0)

#define MEMBLOCK_ALLOC_FLAG_BELOW_4G (1 << 0)

typedef struct
{
  /* [start, end) */
  uint64_t start, end;
  uint8_t type;
  uint8_t pad[7];
} memblock_t;

memblock_t *memblocks_iterate (memblock_t *memblock);

size_t memblocks_get_size (void);

int memblock_has_capacity (void);

int memblock_append (uint64_t start, uint64_t end, memblock_type_t type);

void memblocks_clear (void);

/**
 * @Returns: < 0: Failure
               0: Reserved All
               1: Reserved Some (Partially Contained by Stricter Region(s))
               2: Reserved None (Contained by Stricter Region)
 */
int memblock_reserve (uint64_t start, uint64_t end, memblock_type_t type,
                      int flags);

int memblock_allocate (uint64_t *start, uint64_t size, int flags);

#endif