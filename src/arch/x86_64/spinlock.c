#include "spinlock.h"

void
acquire_spinlock (spinlock_t *spinlock)
{
  int eax;

acquire:
  __asm__ volatile ("lock cmpxchgl %3, %1"
                    : "=a"(eax), "+m"(spinlock->lock)
                    : "a"(0), "b"(1)
                    : "memory");

  if (!eax)
    return;

  while (spinlock->lock)
    __asm__ volatile ("pause" ::: "memory");

  goto acquire;
}

void
release_spinlock (spinlock_t *spinlock)
{
  __asm__ volatile ("movl $0, %0" : "=m"(spinlock->lock)::"memory");
}