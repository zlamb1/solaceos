#include <stdint.h>

unsigned long long __udivdi3 (unsigned long long a, unsigned long long b);
unsigned long long __umoddi3 (unsigned long long a, unsigned long long b);

unsigned long long
__udivdi3 (unsigned long long n, unsigned long long d)
{
  unsigned long long q, r;
  if (d == 0)
    /* trigger exception */
    return (uint32_t) n / (uint32_t) d;
  if (d > n)
    return 0;
  else if (d == n)
    return 1;
  q = 0;
  r = 0;
  (void) r;
  for (int i = 63; i >= 0; i--)
    {
      r <<= 1;
      r |= (n >> i) & 0x1;
      if (r >= d)
        {
          r -= d;
          q |= (1LLU << i);
        }
    }
  return q;
}

unsigned long long
__umoddi3 (unsigned long long n, unsigned long long d)
{
  unsigned long long q, r;
  if (d == 0)
    /* trigger exception */
    return (unsigned long) n / (unsigned long) d;
  if (d > n)
    return n;
  else if (d == n)
    return 0;
  q = 0;
  r = 0;
  (void) q;
  for (int i = 63; i >= 0; i--)
    {
      r <<= 1;
      r |= (n >> i) & 0x1;
      if (r >= d)
        {
          r -= d;
          q |= (1LLU << i);
        }
    }
  return r;
}