#include "mem.h"

void
kmemcpy (void *dst, const void *src, size_t count)
{
  char *d       = (char *) dst;
  const char *s = (const char *) src;

  for (size_t i = 0; i < count; i++)
    d[i] = s[i];
}

void
kmemset (void *dst, int v, size_t count)
{
  char *d = (char *) dst;

  for (size_t i = 0; i < count; i++)
    d[i] = (char) v;
}