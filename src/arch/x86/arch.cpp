#include "arch.hpp"

void Arch::sfence() { __asm__ volatile("sfence" ::: "memory"); }