#include "endian.h"

uint16_t
le16toh (__le16 n)
{
  return n;
}

__le16
htole16 (uint16_t n)
{
  return n;
}

uint16_t
be16toh (__be16 n)
{
  return n >> 8 | n << 8;
}

__be16
htobe16 (uint16_t n)
{
  return n << 8 | n >> 8;
}

uint32_t
le32toh (__le32 n)
{
  return n;
}

__le32
htole32 (uint32_t n)
{
  return n;
}

uint32_t
be32toh (__be32 n)
{
  __asm__ volatile ("bswapl %0" : "+g"(n));
  return n;
}

__be32
htobe32 (uint32_t n)
{
  __asm__ volatile ("bswapl %0" : "+g"(n));
  return n;
}

uint64_t
le64toh (__le64 n)
{
  return n;
}

__le64
htole64 (uint64_t n)
{
  return n;
}

uint64_t
be64toh (__be64 n)
{
  __asm__ volatile ("bswapq %0" : "+g"(n));
  return n;
}

__be64
htobe64 (uint64_t n)
{
  __asm__ volatile ("bswapq %0" : "+g"(n));
  return n;
}