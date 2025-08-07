#ifndef PALLOC_H
#define PALLOC_H 1

#include "ldr/boot.h"

void palloc_init (boot_info_t *boot_info);

void *alloc_page (void);
void free_page (void *p);

#endif