#ifndef VMM_H
#define VMM_H 1

#include <stdint.h>

#include "spinlock.h"

#define VMA_FLAG_READ  (1 << 0)
#define VMA_FLAG_WRITE (1 << 1)
#define VMA_FLAG_EXEC  (1 << 2)

typedef struct
{
  uint64_t start, end;
  uint8_t vma_flags;
} vm_area_t;

typedef struct
{
  uint64_t start, end;
  vm_area_t *areas;
  spinlock_t lock;
} vmm_t;

#endif