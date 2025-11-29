#include "smp/irq.hpp"
#include "common.h"

namespace IRQ {

int Save() {
  u64 irq;
  __asm__ volatile("pushf; pop %0" : "=m"(irq)::"memory");
  return (irq & 0x200) ? 1 : 0;
}

void Enable() { __asm__ volatile("sti" ::: "memory", "cc"); }

void Disable() { __asm__ volatile("cli" ::: "memory", "cc"); }

void Restore(int irq) { irq ? Enable() : Disable(); }

} // namespace IRQ
