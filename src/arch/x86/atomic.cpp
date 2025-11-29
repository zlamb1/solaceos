#include "smp/atomic.hpp"
#include "common.h"

template <> template <> int Atomic<int>::Load<MemoryOrder::Relaxed>() const {
  int i;
  __asm__ volatile("mov %1, %0" : "=r"(i) : "m"(m_value) : "memory");
  return i;
}

template <> template <> int Atomic<int>::Load<MemoryOrder::Acquire>() const {
  return Load<MemoryOrder::Relaxed>();
}

template <>
template <>
void Atomic<int>::Store<MemoryOrder::Release>(int desired) {
  __asm__ volatile("mov %1, %0" : "=m"(m_value) : "r"(desired) : "memory");
}

template <>
template <>
bool Atomic<int>::CompareExchangeWeak<MemoryOrder::Acquire>(int &expected,
                                                            int desired) {
  u8 zf;
  __asm__ volatile("lock cmpxchg %3, %0; sete %b2"
                   : "+m"(m_value), "+a"(expected), "=q"(zf)
                   : "r"(desired)
                   : "memory");
  return zf == 1;
}