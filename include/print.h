#ifndef PRINT_H
#define PRINT_H 1

#include <stdarg.h>

#include "limine/limine.h"

void kprint_init (struct limine_framebuffer_response *);

int vkprint (const char *fmt, va_list args);
int kprint (const char *fmt, ...);
int kprintln (const char *fmt, ...);

#endif