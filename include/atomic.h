#ifndef ATOMIC_H
#define ATOMIC_H 1

typedef int atomic_t;

int atomic_get (const atomic_t *x);
int atomic_get_acquire (const atomic_t *x);

void atomic_set (atomic_t *x, int n);
void atomic_set_release (atomic_t *x, int n);

int atomic_cmpxchg_acquire (atomic_t *x, int expected, int n);

#endif