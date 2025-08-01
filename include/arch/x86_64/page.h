#ifndef SOLACE_X86_64_PAGE_H
#define SOLACE_X86_64_PAGE_H 1

#include <stdint.h>

extern uint64_t pml4tBoot[512], pdptBoot[512], pdtBoot[512], ptBoot[512];

#endif
