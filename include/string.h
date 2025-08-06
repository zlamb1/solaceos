#ifndef STRING_H
#define STRING_H 1

#include <stddef.h>

size_t strlen (const char *s);
int strcmp (const char *s1, const char *s2);

void memcpy (void *restrict dst, const void *restrict src, size_t n);
void memset (void *dst, int value, size_t n);
void memmove (void *dst, const void *src, size_t n);

#endif
