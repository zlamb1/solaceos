#ifndef SPINLOCK_H
#define SPINLOCK_H 1

#include <stdint.h>

#include "atomic.h"

typedef struct
{
  atomic_t lock;
} spinlock_t;

typedef unsigned long lockflags_t;

#define SPINLOCK_INIT (spinlock_t){ .lock = 0 }

void spin_lock (spinlock_t *spinlock);
void spin_lock_irq (spinlock_t *spinlock);
lockflags_t spin_lock_irqsave (spinlock_t *spinlock);

void spin_unlock (spinlock_t *spinlock);
void spin_unlock_irq (spinlock_t *spinlock);
void spin_unlock_irqrestore (spinlock_t *spinlock, lockflags_t flags);

#endif