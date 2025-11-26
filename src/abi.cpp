#include "common.h"
#include "new.hpp"

#define ATEXIT_FUNCTION_CAP 128

typedef void (*destructor_type)(void *);

struct atexit_func_entry {
  destructor_type destructor;
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

static int __dso_handle_a;
void *__dso_handle = &__dso_handle_a;

int __cxa_atexit(destructor_type f, void *obj, void *dso) {
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