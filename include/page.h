#ifndef PAGE_H
#define PAGE_H 1

#include <stdint.h>

#ifndef PAGE_SIZE
#define PAGE_SIZE 4096
#endif

#define PT_OFF  2
#define PT_MASK (3 << PT_OFF)

#define PT_NONE (0 << PT_OFF)
#define PT_HEAD (1 << PT_OFF)
#define PT_TAIL (2 << PT_OFF)
#define PT_SLAB (3 << PT_OFF)

#define PF_LOCK (1 << 0)
#define PF_FREE (1 << 1)
#define PF_TYPE (1 << 2)

struct slab;

typedef struct page
{
  uint8_t flags;
  union
  {
    void *p;
    struct page *head;
    struct slab *slab;
  };
} page_t;

#endif