#include <stdint.h>

#include "string.h"

size_t
strlen (const char *s)
{
  size_t len;
  for (len = 0; s[len]; len++)
    ;
  return len;
}

int
strcmp (const char *s1, const char *s2)
{
  unsigned char ch1 = *s1, ch2 = *s2;
  while (ch1 != '\0' && ch1 == ch2)
    {
      ch1 = *++s1;
      ch2 = *++s2;
    }
  return ch1 - ch2;
}

void
memcpy (void *restrict dst, const void *restrict src, size_t n)
{
  char *restrict d = (char *restrict) dst;
  const char *restrict s = (const char *restrict) src;
  for (size_t i = 0; i < n; i++)
    d[i] = s[i];
}

void
memset (void *dst, int value, size_t n)
{
  char *d = (char *) dst;
  for (size_t i = 0; i < n; i++)
    d[i] = (char) value;
}

void
memmove (void *dst, const void *src, size_t n)
{
  unsigned char *d = (unsigned char *) dst;
  const unsigned char *s = (const unsigned char *) src;
  if ((uintptr_t) src < (uintptr_t) dst)
    while (n--)
      d[n] = s[n];
  else
    while (n--)
      *d++ = *s++;
}