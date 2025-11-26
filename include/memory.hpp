#ifndef MEM_H
#define MEM_H 1

#include "common.h"
#include "compiler.h"

#define MEMORY_OPERATIONS_C_IMPL 1

namespace Memory {

#ifndef MEMORY_OPERATIONS_C_IMPL

void *Copy(void *RESTRICT dst, void *RESTRICT src, usize len);
void *Move(void *dst, void *src, usize len);

#else

static inline void *Copy(void *RESTRICT dst, void *RESTRICT src, usize len) {
  auto d = static_cast<char *>(dst);
  auto s = static_cast<const char *>(src);

  for (usize i = 0; i < len; ++i)
    d[i] = s[i];

  return dst;
}

static inline void *Move(void *dst, void *src, usize len) {
  auto _d = reinterpret_cast<uintptr_t>(dst);
  auto _s = reinterpret_cast<uintptr_t>(src);

  auto d = static_cast<char *>(dst);
  auto s = static_cast<const char *>(src);

  // WARNING: This only works on archs with a flat memory model.
  if (_s < _d) {
    for (usize i = len; i > 0; --i)
      d[i - 1] = s[i - 1];
  } else
    for (usize i = 0; i < len; ++i)
      d[i] = s[i];

  return dst;
}

#endif

}; // namespace Memory

#endif