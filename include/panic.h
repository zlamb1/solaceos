#ifndef PANIC_H
#define PANIC_H 1

void __attribute__ ((noreturn)) kpanic (const char *msg);

#endif