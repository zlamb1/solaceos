#include "compiler.h"
#include "limine/limine.h"

LIMINE_REQUESTS_START_MARKER

static struct limine_framebuffer_request fb_request =
    LIMINE_FRAMEBUFFER_REQUEST;

LIMINE_REQUESTS_END_MARKER

extern "C" NORETURN void KernelMain(void);

void KernelMain(void) {
  if (fb_request.response && fb_request.response->framebuffer_count) {
    limine_framebuffer *fb       = fb_request.response->framebuffers[0];
    ((uint8_t *) fb->address)[0] = 255;
  }

  for (;;)
    ;
}