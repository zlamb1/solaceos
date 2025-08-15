#include "spinlock.h"

void
spin_lock (spinlock_t *spinlock)
{
  uint32_t eax;

acquire:
  __asm__ volatile ("lock cmpxchgl %3, %1"
                    : "=a"(eax), "+m"(spinlock->lock)
                    : "a"(0), "r"(1)
                    : "memory");

  if (!eax)
    return;

  while (spinlock->lock)
    __asm__ volatile ("pause" ::: "memory");

  goto acquire;
}

void
spin_lock_irq (spinlock_t *spinlock)
{
  __asm__ volatile ("cli" ::: "memory", "cc");
  spin_lock (spinlock);
}

lockflags_t
spin_lock_irqsave (spinlock_t *spinlock)
{
  lockflags_t flags;

  __asm__ volatile ("pushf; pop %0; cli" : "=r"(flags)::"memory", "cc");
  spin_lock (spinlock);

  return flags;
}

void
spin_unlock (spinlock_t *spinlock)
{
  __asm__ volatile ("" ::: "memory");
  spinlock->lock = 0;
}

void
spin_unlock_irq (spinlock_t *spinlock)
{
  spin_unlock (spinlock);
  __asm__ volatile ("sti" ::: "memory", "cc");
}

void
spin_unlock_irqrestore (spinlock_t *spinlock, lockflags_t flags)
{
  spin_unlock (spinlock);
  __asm__ volatile ("push %0; popf" ::"r"(flags) : "memory", "cc");
}