#include <stddef.h>

#include "limine.h"
#include "print.h"
#include "vcon.h"

LIMINE_REQUESTS_START_MARKER

struct limine_framebuffer_request fb_request = LIMINE_FRAMEBUFFER_REQUEST;

LIMINE_REQUESTS_END_MARKER

__attribute__ ((noreturn)) void kmain (void);

void
kmain (void)
{
  kprint_init (fb_request.response);
  kprintln ("Starting SolaceOS...");

  for (;;)
    ;
}