#ifndef MEM_H
#define MEM_H 1

#include <stddef.h>

void kmemcpy (void *dst, const void *src, size_t count);
void kmemset (void *dst, int val, size_t count);

#endif