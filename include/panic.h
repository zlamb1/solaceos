#ifndef PANIC_H
#define PANIC_H 1

#include "compiler.h"

void NORETURN KernelPanic(const char *msg, ...);

#endif
