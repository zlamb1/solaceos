#include <stdint.h>

uint64_t __udivdi3 (uint64_t a, uint64_t b);
uint64_t __umoddi3 (uint64_t a, uint64_t b);

uint64_t
__udivdi3 (uint64_t n, uint64_t d)
{
  uint64_t q, r;
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
          q |= (1 << i);
        }
    }
  return q;
}

uint64_t
__umoddi3 (uint64_t n, uint64_t d)
{
  uint64_t q, r;
  if (d == 0)
    /* trigger exception */
    return (uint32_t) n / (uint32_t) d;
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
          q |= (1 << i);
        }
    }
  return r;
}