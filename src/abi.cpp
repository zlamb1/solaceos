#include "common.h"

#define ATEXIT_FUNCTION_CAP 128

typedef void ctor_type(void);
typedef void (*dtor_type)(void *);

struct atexit_func_entry {
  dtor_type destructor;
  void *obj;
  void *dso_handle;
};

static struct atexit_func_entry atexit_func_table[ATEXIT_FUNCTION_CAP];
static int atexit_func_count = 0;

/*
 * WARNING: Implemented to satisfy linker. Should never be called unless
 * kmalloc/kfree are implemented. Should panic until then.
 */

void operator delete(void *) {
  for (;;)
    ;
}

void operator delete[](void *) {
  for (;;)
    ;
}

void operator delete(void *, usize) {
  for (;;)
    ;
}

void operator delete[](void *, usize) {
  for (;;)
    ;
}

extern "C" {

/* C / C++ Constructors Init */

extern void (*__preinit_array_start[])(void) __attribute__((weak));
extern void (*__preinit_array_end[])(void) __attribute__((weak));
extern void (*__init_array_start[])(void) __attribute__((weak));
extern void (*__init_array_end[])(void) __attribute__((weak));

extern "C++" void InvokeConstructors() {
  usize count;

  count = __preinit_array_end - __preinit_array_start;

  for (usize i = 0; i < count; ++i)
    __preinit_array_end[i]();

  count = __init_array_end - __init_array_start;

  for (usize i = 0; i < count; ++i)
    __init_array_start[i]();
}

/* C / C++ Destructors Init */

static int __dso_handle_a;
void *__dso_handle = &__dso_handle_a;

int __cxa_atexit(dtor_type f, void *obj, void *dso) {
  if (atexit_func_count >= ATEXIT_FUNCTION_CAP)
    return -1;
  auto ent        = atexit_func_table + atexit_func_count;
  ent->destructor = f;
  ent->obj        = obj;
  ent->dso_handle = dso;
  ++atexit_func_count;
  return 0;
}

void __cxa_finalize(void *dso) {
  int i = atexit_func_count - 1;

  if (!dso) {
    while (i >= 0) {
      auto ent = atexit_func_table[i];
      if (ent.destructor)
        ent.destructor(ent.obj);
      --i;
    }

    atexit_func_count = 0;

    return;
  }

  while (i >= 0) {
    auto ent = atexit_func_table[i];

    if (ent.dso_handle == dso) {
      ent.destructor(ent.obj);
      for (int j = i + 1; j < atexit_func_count; ++j)
        atexit_func_table[j - 1] = atexit_func_table[j];
      --atexit_func_count;
    }

    --i;
  }
}
}