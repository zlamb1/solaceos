#define NORETURN __attribute__((noreturn))

extern "C" NORETURN void kmain(void);

void kmain(void) {
  for (;;)
    ;
}