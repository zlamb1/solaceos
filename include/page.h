#ifndef PAGE_H
#define PAGE_H 1

#include <stdint.h>

#ifndef PAGE_SIZE
#define PAGE_SIZE 4096
#endif

typedef struct
{
  uint8_t flags;
} page_t;

#endif