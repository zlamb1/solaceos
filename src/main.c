#include <stddef.h>

#include "datetime.h"
#include "limine/limine.h"
#include "print.h"

LIMINE_REQUESTS_START_MARKER

static struct limine_framebuffer_request fb_request
    = LIMINE_FRAMEBUFFER_REQUEST;
static struct limine_date_at_boot_request date_request
    = LIMINE_DATE_AT_BOOT_REQUEST;

LIMINE_REQUESTS_END_MARKER

__attribute__ ((noreturn)) void kmain (void);

static void
kboot_time (void)
{
  struct datetime dt = get_datetime (date_request.response->timestamp);
  u32 hour           = dt.hour % 12;
  u32 minute         = dt.minute;

  if (minute > 9)
    kprintln ("Booted At %u:%u %u/%u/%u", hour ? hour : 12, minute,
              dt.month + 1, dt.day_of_month + 1, dt.year);
  else
    kprintln ("Booted At %u:0%u %u/%u/%u", hour ? hour : 12, minute,
              dt.month + 1, dt.day_of_month + 1, dt.year);
}

void
kmain (void)
{
  kprint_init (fb_request.response);
  kprintln ("Starting SolaceOS...");
  kboot_time ();

  for (;;)
    ;
}