#ifndef SPINLOCK_H
#define SPINLOCK_H 1

typedef struct
{
    int lock;
} spinlock_t;

void acquire_spinlock(spinlock_t *spinlock);
void release_spinlock(spinlock_t *spinlock);

#endif