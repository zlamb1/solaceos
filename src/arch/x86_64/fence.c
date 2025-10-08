#include "arch/fence.h"

void
sfence (void)
{
  __asm__ volatile ("sfence" ::: "memory");
}