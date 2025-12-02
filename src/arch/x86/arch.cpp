#include "arch.hpp"

void Arch::StoreFence() { __asm__ volatile("sfence" ::: "memory"); }

void Arch::BusyWaiting() { __asm__ volatile("pause" ::: "memory"); }