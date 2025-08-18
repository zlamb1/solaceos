#ifndef COMPILER_H
#define COMPILER_H 1

#define __always_inline __attribute__ ((always_inline))
#define __maybe_unused  __attribute__ ((unused))

#define barrier() __asm__ volatile ("" ::: "memory")

#endif