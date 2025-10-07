#ifndef COMMON_H
#define COMMON_H 1

#include <stddef.h>
#include <stdint.h>

#define always_inline inline __attribute__ ((always_inline))

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef size_t usize;

#endif