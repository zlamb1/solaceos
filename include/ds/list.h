#include "types.h"
#ifndef LIST_H
#define LIST_H 1

#include <stddef.h>

#include "compiler.h"

#define container_of(ptr, type, member)                                       \
  ({                                                                          \
    void *__mptr = (void *) (ptr);                                            \
    _Static_assert (__builtin_types_compatible_p (                            \
                        typeof (*(ptr)), typeof (((type *) 0)->member)),      \
                    "container_of: incompatible types");                      \
    ((type *) (__mptr - offsetof (type, member)));                            \
  })

#define LIST_FOR_EACH(head)                                                   \
  for (list_t *element = (head)->next; element != (head);                     \
       element = element->next)

#define LIST_HEAD(head) (list_t){ .prev = &(head), .next = &(head) };

typedef struct list
{
  struct list *prev, *next;
} list_t;

static __always_inline __maybe_unused void
list_init (list_t *head)
{
  head->prev = head;
  head->next = head;
}

static __always_inline __maybe_unused void
list_add (list_t *head, list_t *element)
{
  element->prev = head;
  element->next = head->next;
  head->next = element;
}

static __always_inline __maybe_unused void
list_remove (list_t *element)
{
  element->prev->next = element->next;
  element->next->prev = element->prev;
  element->prev = NULL;
  element->next = NULL;
}

static __always_inline __maybe_unused u32
list_get_size (list_t *head)
{
  u32 size = 0;

  LIST_FOR_EACH (head) { ++size; }

  return size;
}

static __always_inline __maybe_unused int
list_is_empty (list_t *head)
{
  return head->next == head;
}

static __always_inline __maybe_unused list_t *
list_get_tail (list_t *head)
{
  return head->prev;
}

#endif