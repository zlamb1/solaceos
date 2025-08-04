#include "memblock.h"
#include "errno.h"

#define MEMBLOCKS_CAP 32

size_t nmemblocks = 0;
memblock_t memblocks[MEMBLOCKS_CAP];

int
memblock_has_capacity (void)
{
  return nmemblocks < MEMBLOCKS_CAP - 1;
}

int
memblock_append (uint64_t start, uint64_t end, memblock_type_t type)
{
  if (start >= end || type > MEMBLOCK_TYPE_MAX)
    return -EINVAL;
  if (!memblock_has_capacity ())
    return -ENOMEM;
  memblocks[nmemblocks++]
      = (memblock_t){ .start = start, .end = end, .type = type };
  return 0;
}