#include "mm/memblock.h"
#include "errno.h"
#include "ldr/fail.h"

#define MEMBLOCKS_CAP 256

size_t nmemblocks = 0;
memblock_t memblocks[MEMBLOCKS_CAP];

memblock_t *
memblocks_iterate (memblock_t *memblock)
{
  if (memblock == NULL)
    {
      if (nmemblocks)
        return memblocks;
      else
        return NULL;
    }
  if (!nmemblocks)
    fail ("memblocks_iterate called with zero memblocks");
  if (memblock == memblocks + (nmemblocks - 1))
    return NULL;
  return memblock + 1;
}

size_t
memblocks_get_size (void)
{
  return nmemblocks;
}

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
      = (memblock_t) { .start = start, .end = end, .type = type };
  return 0;
}

void
memblocks_clear (void)
{
  nmemblocks = 0;
}