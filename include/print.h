#ifndef PRINT_H
#define PRINT_H 1

#include <stdarg.h>
#include <stddef.h>

int kvsnprintf (char *buf, size_t size, const char *fmt, va_list args);

#endif