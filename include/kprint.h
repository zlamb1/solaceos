#ifndef KPRINT_H
#define KPRINT_H 1

#include <stdarg.h>

#include "console.h"

#define KPRINT_INFO "\xC0"

void initprint (console_t *con);

int kvprintf (const char *fmt, va_list args);
int __attribute__ ((format (printf, 1, 2))) kprintf (const char *fmt, ...);

#endif