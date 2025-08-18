#include "mm/memblock.h"
#include "errno.h"
#include "sort.h"
#include "util.h"

#define MEMBLOCK_EVT_TYPE_START 0
#define MEMBLOCK_EVT_TYPE_END   1

typedef struct
{
  uint64_t value;
  uint8_t type;
  uint8_t evt_type;
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
memblock_reserve (uint64_t start, uint64_t end, memblock_type_t type,
                  int flags)
{
  memblock_t *memblock = NULL;
  memblock_event_t evts[(memblocks_get_size () << 1) + 2];
  uint64_t pos = 0, reserved_length = 0;
  int nevts = 0, active_type = -1;
  unsigned char types[MEMBLOCK_TYPE_MAX] = { 0 };
  if (start >= end || type >= MEMBLOCK_TYPE_MAX)
    return -EINVAL;
  if (!memblock_has_capacity ())
    return -ENOMEM;
  while ((memblock = memblocks_iterate (memblock)) != NULL)
    {
      evts[nevts++]
          = (memblock_event_t) { .value = memblock->start,
                                 .type = memblock->type,
                                 .evt_type = MEMBLOCK_EVT_TYPE_START };
      evts[nevts++] = (memblock_event_t) { .value = memblock->end,
                                           .type = memblock->type,
                                           .evt_type = MEMBLOCK_EVT_TYPE_END };
    }
  evts[nevts++] = (memblock_event_t) {
    .value = start,
    .type = flags & MEMBLOCK_RESERVE_FLAG_FORCE ? MEMBLOCK_TYPE_MAX : type,
    .evt_type = MEMBLOCK_EVT_TYPE_START
  };
  evts[nevts++] = (memblock_event_t) {
    .value = end,
    .type = flags & MEMBLOCK_RESERVE_FLAG_FORCE ? MEMBLOCK_TYPE_MAX : type,
    .evt_type = MEMBLOCK_EVT_TYPE_END
  };
  ssort (evts, nevts, sizeof (memblock_event_t), memblock_event_cmp);
  memblocks_clear ();
  for (int i = 0; i < nevts; i++)
    {
      memblock_event_t evt = evts[i];
      int effective_type
          = active_type == MEMBLOCK_TYPE_MAX ? (int) type : active_type;
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
              if ((uint32_t) effective_type == type && pos < end
                  && evt.value > start)
                reserved_length += MIN (end, evt.value) - MAX (start, pos);
              memblock_append (pos, evt.value, effective_type);
              pos = evt.value;
              active_type = evt.type;
            }
          continue;
        }
      if (!--types[evt.type] && evt.type == (uint32_t) active_type)
        {
          if ((uint32_t) effective_type == type && pos < end
              && evt.value > start)
            reserved_length += MIN (end, evt.value) - MAX (start, pos);
          memblock_append (pos, evt.value, effective_type);
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
  if (!reserved_length)
    return 2;
  else if (reserved_length == end - start)
    return 0;
  else
    return 1;
}

int
memblock_allocate (uint64_t *start, uint64_t size, int flags)
{
  memblock_t *memblock = NULL;
  int result;
  if (start == NULL)
    return -EINVAL;
  size = (size + 4095) >> 12 << 12;

  while ((memblock = memblocks_iterate (memblock)) != NULL)
    {
      uint64_t block_start = memblock->start, block_end = memblock->end;
      if (memblock->type != MEMBLOCK_TYPE_FREE
          || size > block_end - block_start)
        continue;
      if (flags & MEMBLOCK_ALLOC_FLAG_BELOW_4G)
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
      block_start = ALIGN_UP (block_start, 4096);
      if (size > block_end - block_start)
        continue;
      result = memblock_reserve (block_start, block_start + size,
                                 MEMBLOCK_TYPE_ALLOCATED, 0);
      if (result)
        return result;
      *start = block_start;
      return 0;
    }

  return -ENOMEM;
}