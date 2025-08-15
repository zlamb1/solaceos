#ifndef KPRINT_H
#define KPRINT_H 1

#include <stdarg.h>

#include "console.h"
#include "spinlock.h"

#define KPRINT_INFO "\xC0"
#define KPRINT_WARN "\xC1"
#define KPRINT_ERR  "\xFF"

typedef struct
{
  console_t *console;
  size_t head, tail;
  char *log;
  spinlock_t lock;
} kern_log_t;

extern kern_log_t kern_log;

void initprint (console_t *con);

int kvprintf (const char *fmt, va_list args);
int __attribute__ ((format (printf, 1, 2))) kprintf (const char *fmt, ...);

#endif