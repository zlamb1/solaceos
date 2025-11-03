#include "limine/limine.h"
#ifndef PANIC_H
#define PANIC_H 1

void kpanic_init (struct limine_framebuffer *fb);
void __attribute__ ((noreturn)) kpanic (const char *msg);

#endif