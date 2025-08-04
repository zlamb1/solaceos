#ifndef SORT_H
#define SORT_H 1

#include <stddef.h>

void ssort (void *arr, size_t nelements, size_t elsize,
            int (*cmp) (const void *, const void *));

#endif