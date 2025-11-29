#ifndef COMPILER_H
#define COMPILER_H 1

#ifndef __GNUC__
#error UNSUPPORTED COMPILER
#endif

#define NORETURN __attribute__((noreturn))
#define PACKED   __attribute__((packed))
#define RESTRICT __restrict__
#define UNUSED   __attribute__((unused))

#define cfence() __asm__ volatile("" ::: "memory")

#endif