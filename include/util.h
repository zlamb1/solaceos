#ifndef UTIL_H
#define UTIL_H 1

#ifndef MAX
#define MAX(X, Y) ((X) > (Y) ? (X) : (Y))
#endif

#ifndef offsetof
#define offsetof(type, member) __builtin_offsetof (type, member)
#endif

#define container_of(ptr, type, member)                                       \
  ({                                                                          \
    const typeof (((type *) 0)->member) *__mptr = (ptr);                      \
    _Pragma ("GCC diagnostic push") _Pragma (                                 \
        "GCC diagnostic ignored \"-Wcast-qual\"") (type *)                    \
        __builtin_assume_aligned ((char *) __mptr - offsetof (type, member),  \
                                  __alignof__ (type));                        \
    _Pragma ("GCC diagnostic pop")                                            \
  })

#endif