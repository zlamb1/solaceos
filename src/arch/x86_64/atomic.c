#include "atomic.h"

#define compiler_fence() __asm__ volatile ("" ::: "memory")

int
atomic_get (const atomic_t *x)
{
  return *((const volatile atomic_t *) x);
}

int
atomic_get_acquire (const atomic_t *x)
{
  compiler_fence ();
  return *((const volatile atomic_t *) x);
}

void
atomic_set (atomic_t *x, int n)
{
  *((volatile atomic_t *) x) = n;
}

void
atomic_set_release (atomic_t *x, int n)
{
  compiler_fence ();
  *((volatile atomic_t *) x) = n;
}

int
atomic_cmpxchg_acquire (atomic_t *x, int expected, int n)
{
  __asm__ volatile ("lock cmpxchgl %3, %1"
                    : "=a"(expected), "+m"(*x)
                    : "a"(expected), "r"(n)
                    : "memory");

  return expected;
}