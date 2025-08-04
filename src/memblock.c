#include "memblock.h"
#include "errno.h"
#include "sort.h"

#define MEMBLOCK_EVT_TYPE_START 0
#define MEMBLOCK_EVT_TYPE_END   1

typedef struct
{
  uint64_t value;
  uint32_t type;
  uint32_t evt_type;
} memblock_event_t;

static int
memblock_event_cmp (const void *e1, const void *e2)
{
  const memblock_event_t *evt1 = e1, *evt2 = e2;
  if (evt1->value != evt2->value)
    return evt1->value < evt2->value ? -1 : 1;
  return evt1->evt_type - evt2->evt_type;
}

int
memblock_reserve (uint64_t start, uint64_t end, memblock_type_t type)
{
  uint64_t pos = 0;
  int nevts = 0, active_type = -1;
  memblock_event_t evts[(nmemblocks << 1) + 2];
  unsigned char types[MEMBLOCK_TYPE_MAX] = { 0 };
  if (start >= end)
    return -EINVAL;
  if (!memblock_has_capacity ())
    return -ENOMEM;
  for (size_t i = 0; i < nmemblocks; i++)
    {
      memblock_t *memblock = memblocks + i;
      evts[nevts++]
          = (memblock_event_t){ .value = memblock->start,
                                .type = memblock->type,
                                .evt_type = MEMBLOCK_EVT_TYPE_START };
      evts[nevts++] = (memblock_event_t){ .value = memblock->end,
                                          .type = memblock->type,
                                          .evt_type = MEMBLOCK_EVT_TYPE_END };
    }
  evts[nevts++] = (memblock_event_t){ .value = start,
                                      .type = type,
                                      .evt_type = MEMBLOCK_EVT_TYPE_START };
  evts[nevts++] = (memblock_event_t){ .value = end,
                                      .type = type,
                                      .evt_type = MEMBLOCK_EVT_TYPE_END };
  ssort (evts, nevts, sizeof (memblock_event_t), memblock_event_cmp);
  nmemblocks = 0;
  for (int i = 0; i < nevts; i++)
    {
      memblock_event_t evt = evts[i];
      if (evt.evt_type == MEMBLOCK_EVT_TYPE_START)
        {
          types[evt.type]++;
          if (active_type == -1)
            {
              pos = evt.value;
              active_type = evt.type;
            }
          else if ((uint32_t) active_type < evt.type)
            {
              memblock_append (pos, evt.value, active_type);
              pos = evt.value;
              active_type = evt.type;
            }
          continue;
        }
      if (!--types[evt.type] && evt.type == (uint32_t) active_type)
        {
          memblock_append (pos, evt.value, active_type);
          pos = evt.value;
          active_type = -1;
          for (int j = evt.type - 1; j >= MEMBLOCK_TYPE_FREE; j--)
            {
              if (types[j])
                {
                  active_type = j;
                  break;
                }
            }
        }
    }
  return 0;
}

int
memblock_allocate (uint64_t *start, uint64_t size, int flags)
{
  if (start == NULL)
    return -EINVAL;
  size = (size + 4095) >> 12 << 12;
  for (size_t cur = 0; cur < nmemblocks; cur++)
    {
      memblock_t *memblock = memblocks + cur;
      uint64_t block_start = memblock->start, block_end = memblock->end;
      if (memblock->type != MEMBLOCK_TYPE_FREE
          || size > block_end - block_start)
        continue;
      if (flags & MEMBLOCK_FLAG_BELOW_4G)
        {
          if (block_start > 0xFFFFFFFF)
            break;
          else if (block_end > 0xFFFFFFFF)
            {
              block_end = 0xFFFFFFFF;
              if (size > block_end - block_start)
                continue;
            }
        }
      *start = block_start;
      memblock->start += size;
      return 0;
    }
  return -ENOMEM;
}