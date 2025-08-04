#include "sort.h"
#include "string.h"

void
ssort (void *arr, size_t nelements, size_t elsize,
       int (*cmp) (const void *, const void *))
{
  char *carr = (char *) arr;
  char tmp[elsize];
  if (nelements == 0)
    return;
  for (size_t i = 0; i < nelements - 1; i++)
    {
      size_t min_index = i;
      for (size_t j = i + 1; j < nelements; j++)
        {
          const void *cur_el = carr + j * elsize;
          const void *min_el = carr + min_index * elsize;
          if (cmp (cur_el, min_el) < 0)
            min_index = j;
        }
      if (min_index != i)
        {
          char *cur_el = carr + i * elsize;
          char *min_el = carr + min_index * elsize;
          memcpy (tmp, cur_el, elsize);
          memcpy (cur_el, min_el, elsize);
          memcpy (min_el, tmp, elsize);
        }
    }
}