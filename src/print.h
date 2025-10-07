#ifndef PRINT_H
#define PRINT_H 1

#include "limine.h"

void kprint_init (struct limine_framebuffer_response *);

void kprint (const char *fmt);
void kprintln (const char *fmt);

#endif