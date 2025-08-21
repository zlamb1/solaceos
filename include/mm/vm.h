#ifndef VMM_H
#define VMM_H 1

#include <stdint.h>

#include "ds/list.h"
#include "spinlock.h"
#include "types.h"

#define VMA_FLAG_READ  (1 << 0)
#define VMA_FLAG_WRITE (1 << 1)
#define VMA_FLAG_EXEC  (1 << 2)

typedef struct
{
  uint64_t vm_start;
  uint64_t vm_end;
  uint8_t vm_flags;
} vm_area_t;

typedef struct
{
  u64 vmm_start;
  u64 vmm_end;
  list_t vmm_areas;
  void *pt;
  spinlock_t vmm_lock;
} vmm_t;

void vmm_init (vmm_t *vmm, void *pt);
void vmm_init_pt (vmm_t *vmm);

#endif