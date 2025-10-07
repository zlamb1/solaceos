#include <limits.h>

#include "print.h"
#include "vcon.h"

static struct vcon con;

enum length_type
{
  LENGTH_TYPE_NONE,
  LENGTH_TYPE_LONG,
  LENGTH_TYPE_LONGLONG,
  LENGTH_TYPE_INTMAX,
  LENGTH_TYPE_SIZE,
  LENGTH_TYPE_PTRDIFF,
};

void
kprint_init (struct limine_framebuffer_response *fb_response)
{
  struct limine_framebuffer *fb = NULL;

  for (uint64_t i = 0; i < fb_response->framebuffer_count; i++)
    {
      struct limine_framebuffer *_fb = fb_response->framebuffers[i];

      if (_fb->memory_model != LIMINE_FRAMEBUFFER_RGB)
        continue;

      fb = _fb;
      break;
    }

  if (!fb)
    return;

  con = vcon_init (fb);
}

static int
kprint_u64 (u64 n, u8 base, u8 capital)
{
  int len = 0;
  char buf[64];

  if (base < 2 || base > 16)
    return -1;

  if (!n)
    {
      vcon_putchar (&con, '0');
      return 1;
    }

  u64 rem;

  while (n)
    {
      rem        = n % base;
      n          = n / base;
      buf[len++] = (capital ? "0123456789ABCDEF" : "0123456789abcdef")[rem];
    }

  for (int i = len - 1; i >= 0; i--)
    vcon_putchar (&con, buf[i]);

  return len;
}

static int
kprint_i64 (i64 n, u8 base, u8 capital)
{
  return n < 0
             ? (vcon_putchar (&con, '-'),
                1
                    + kprint_u64 (n == LONG_LONG_MIN ? (1LLU << 63) : (u64) -n,
                                  base, capital))
             : kprint_u64 ((u64) n, base, capital);
}

int
vkprint (const char *fmt, va_list args)
{
  int len = 0;

  for (char c = fmt[0]; c; c = (++fmt)[0])
    {
      if (c != '%')
        {
          ++len;
          vcon_putchar (&con, c);
          continue;
        }

      c = (++fmt)[0];

      enum length_type length_type = LENGTH_TYPE_NONE;

      switch (c)
        {
        case '%':
          ++len;
          vcon_putchar (&con, '%');
          continue;
        case 'c':
          ++len;
          vcon_putchar (&con, (char) va_arg (args, int));
          continue;
        case 's':
          for (const char *s = va_arg (args, const char *); s[0]; ++s, ++len)
            vcon_putchar (&con, s[0]);
          continue;
        case 'p':
        case 'P':
          {
            u8 capital = c == 'P';
            vcon_putchar (&con, '0');
            vcon_putchar (&con, capital ? 'X' : 'x');
            len += 2
                   + kprint_u64 ((uintptr_t) va_arg (args, void *), 16,
                                 capital);
            continue;
          }
        case 'l':
          c = (++fmt)[0];
          if (c == 'l')
            {
              c           = (++fmt)[0];
              length_type = LENGTH_TYPE_LONGLONG;
            }
          else
            length_type = LENGTH_TYPE_LONG;
          break;
        case 'j':
          c           = (++fmt)[0];
          length_type = LENGTH_TYPE_INTMAX;
          break;
        case 'z':
          c           = (++fmt)[0];
          length_type = LENGTH_TYPE_SIZE;
          break;
        case 't':
          c           = (++fmt)[0];
          length_type = LENGTH_TYPE_PTRDIFF;
          break;
        default:
          break;
        }

      u8 is_signed = 0;
      u8 capital   = 0;
      u8 base      = 10;

      switch (c)
        {
        case 'd':
        case 'i':
          is_signed = 1;
          break;
        case 'u':
          break;
        case 'b':
        case 'B':
          base    = 2;
          capital = c == 'B';
          break;
        case 'o':
        case 'O':
          base    = 8;
          capital = c == 'O';
          break;
        case 'x':
        case 'X':
          base    = 16;
          capital = c == 'X';
          break;
        default:
          return -1;
        }

      switch (length_type)
        {
        case LENGTH_TYPE_NONE:
          if (is_signed)
            len += kprint_i64 (va_arg (args, int), base, capital);
          else
            len += kprint_u64 (va_arg (args, unsigned int), base, capital);
          break;
        case LENGTH_TYPE_LONG:
          if (is_signed)
            len += kprint_i64 (va_arg (args, long), base, capital);
          else
            len += kprint_u64 (va_arg (args, unsigned long), base, capital);
          break;
        case LENGTH_TYPE_LONGLONG:
          if (is_signed)
            len += kprint_i64 (va_arg (args, long long), base, capital);
          else
            len += kprint_u64 (va_arg (args, unsigned long long), base,
                               capital);
          break;
        case LENGTH_TYPE_INTMAX:
          if (is_signed)
            len += kprint_i64 (va_arg (args, intmax_t), base, capital);
          else
            len += kprint_u64 (va_arg (args, uintmax_t), base, capital);
          break;
        case LENGTH_TYPE_SIZE:
          len += kprint_u64 (va_arg (args, size_t), base, capital);
          break;
        case LENGTH_TYPE_PTRDIFF:
          len += kprint_u64 (va_arg (args, ptrdiff_t), base, capital);
          break;
        }
    }

  return len;
}

int
kprint (const char *fmt, ...)
{
  int len;
  va_list args;

  va_start (args, fmt);
  len = vkprint (fmt, args);
  va_end (args);

  return len;
}

int
kprintln (const char *fmt, ...)
{
  int len = 1;
  va_list args;

  va_start (args, fmt);
  len += vkprint (fmt, args);
  kprint ("\n");
  va_end (args);

  return len;
}