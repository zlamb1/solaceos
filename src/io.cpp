#include <climits>
#include <cstdarg>
#include <limits>

#include "io.hpp"
#include "memory.hpp"
#include "smp/irq.hpp"
#include "smp/lock.hpp"

namespace IO {

enum class LengthSpecifier {
  None,
  Half,
  HalfHalf,
  Long,
  LongLong,
  Size,
};

KernelConsole Log;

void KernelConsole::AddConsoleDevice(ConsoleDevice *console, bool write_log) {
  {
    auto consoles_guard = m_lock.AsGuard();
    console->m_next     = m_consoles;
    m_consoles          = console;
  }

  if (write_log) {
    auto klog_guard    = m_klog.m_lock.AsGuard();
    auto console_guard = console->m_lock.AsGuard();
    char *first        = m_klog.m_log + m_klog.m_head;

    if (m_klog.m_head <= m_klog.m_tail) {
      usize len = m_klog.m_tail - m_klog.m_head;
      console->Write<SMP::LockOperation::Unlocked>(first, len);
    } else {
      usize len = m_klog.m_len - m_klog.m_head;
      console->Write<SMP::LockOperation::Unlocked>(first, len);
      console->Write<SMP::LockOperation::Unlocked>(m_klog.m_log, m_klog.m_tail);
    }

    console->Flush<SMP::LockOperation::Unlocked>();
  }
}

void KernelConsole::Flush() {
  auto klog_guard     = m_klog.m_lock.AsGuard();
  auto consoles_guard = m_lock.AsGuard();
  auto console        = m_consoles;

  if (m_klog.m_first <= m_klog.m_tail) {
    usize len   = m_klog.m_tail - m_klog.m_first;
    char *first = m_klog.m_log + m_klog.m_first;

    while (console != nullptr) {
      auto console_guard = console->m_lock.AsGuard();
      console->Write<SMP::LockOperation::Unlocked>(first, len);
      console->Flush<SMP::LockOperation::Unlocked>();
      console = console->m_next;
    }
  } else {
    usize len   = m_klog.m_len - m_klog.m_first;
    char *first = m_klog.m_log + m_klog.m_first;

    while (console != nullptr) {
      auto console_guard = console->m_lock.AsGuard();
      console->Write<SMP::LockOperation::Unlocked>(first, len);
      console->Write<SMP::LockOperation::Unlocked>(m_klog.m_log, m_klog.m_tail);
      console->Flush<SMP::LockOperation::Unlocked>();
      console = console->m_next;
    }
  }

  m_klog.m_first = m_klog.m_tail;
}

void KernelConsole::Write(const char *str, bool flush) {
  if (!m_klog.m_len)
    goto End;

  {
    auto guard = m_klog.m_lock.AsGuard();

    for (; str[0]; ++str)
      m_klog.Write(str[0]);
  }

End:
  if (flush)
    Flush();
}

void KernelConsole::Write(const char *str, usize len, bool flush) {
  if (!len || !m_klog.m_len)
    goto End;

  {
    auto guard = m_klog.m_lock.AsGuard();

    for (usize i = 0; i < len; ++i)
      m_klog.Write(str[i]);
  }

End:
  if (flush)
    Flush();
}

void KernelConsole::Log::Write(char ch) {
  m_log[m_tail++] = ch;
  if (m_tail == m_len)
    m_tail = 0;
  if (m_tail == m_head && ++m_head == m_len) {
    m_head = 0;
  }
}

void KernelConsole::Log::Write(const char *str, usize len) {
  for (usize i = 0; i < len; ++i)
    Write(str[i]);
}

template <unsigned char N> struct TmpBuf {
  char m_buf[N];
  int m_len    = 0;
  usize m_tlen = 0;

  static_assert(N > 0);

  void Write(char ch) {
    ++m_tlen;
    m_buf[m_len++] = ch;
    if (m_len == N) {
      Log.GetLog().Write(m_buf, m_len);
      m_len = 0;
    }
  }

  void Write(const char *str) {
    for (; *str; ++str)
      Write(*str);
  }

  void Write(const char *str, usize len) {
    usize move;

    while (len) {
      if (len < N - (usize) m_len)
        move = len;
      else
        move = N - (usize) m_len;

      Memory::Copy(m_buf + m_len, str, move);

      str += move;
      m_len += move;

      if (m_len == N) {
        Log.GetLog().Write(m_buf, m_len);
        m_len = 0;
      }

      len -= move;
    }
  }

  void Reverse() {
    for (int i = 0; i < m_len >> 1; ++i) {
      char tmp             = m_buf[i];
      m_buf[i]             = m_buf[m_len - 1 - i];
      m_buf[m_len - 1 - i] = tmp;
    }
  }

  void WriteBuf() {
    if (m_len) {
      Log.GetLog().Write(m_buf, m_len);
      m_len = 0;
    }
  }
};

#define TMP_BUF_SIZE 128

int Print(const char *fmt, int flush, va_list args) {
  int result = 0;
  const char *prev;
  char ch;
  TmpBuf<TMP_BUF_SIZE> tmpbuf;
  LengthSpecifier ls;

  Log.GetLog().m_lock.Acquire<IRQ::LockMode::IRQSave>();

Read:
  if (tmpbuf.m_tlen >= std::numeric_limits<int>::max())
    goto End;

  ch = *fmt++;

  if (ch != '%') {
    if (!ch)
      goto End;

    tmpbuf.Write(ch);
    goto Read;
  }

  prev = fmt;
  ch   = *fmt++;

  if (ch == '%') {
    tmpbuf.Write('%');
    goto Read;
  }

  ls = LengthSpecifier::None;

  if (ch == 'h') {
    ls = LengthSpecifier::Half;
    ch = *fmt++;

    if (ch == 'h') {
      ls = LengthSpecifier::HalfHalf;
      ch = *fmt++;
    }
  }

  if (ch == 'l') {
    ls = LengthSpecifier::Long;
    ch = *fmt++;

    if (ch == 'l') {
      ls = LengthSpecifier::LongLong;
      ch = *fmt++;
    }
  }

  if (ch == 'z') {
    ls = LengthSpecifier::Size;
    ch = *fmt++;
  }

  if (ch == 'c') {
    if (ls != LengthSpecifier::None)
      goto Fail;

    tmpbuf.Write(va_arg(args, int));
    goto Read;
  }

  if (ch == 's') {
    if (ls != LengthSpecifier::None)
      goto Fail;

    tmpbuf.Write(va_arg(args, const char *));
    goto Read;
  }

  if (ch == 'd' || ch == 'i' || ch == 'u' || ch == 'b' || ch == 'B' ||
      ch == 'o' || ch == 'x' || ch == 'X' || ch == 'p') {
    bool is_capital    = false;
    unsigned char base = 10;

    long long ll;
    unsigned long long ull;

    if (ch == 'd' || ch == 'i') {
      switch (ls) {
      case LengthSpecifier::None:
        ll = va_arg(args, int);
        break;
      case LengthSpecifier::Half:
        ll = static_cast<short int>(va_arg(args, int));
        break;
      case LengthSpecifier::HalfHalf:
        ll = static_cast<signed char>(va_arg(args, int));
        break;
      case LengthSpecifier::Long:
        ll = va_arg(args, long int);
        break;
      case LengthSpecifier::LongLong:
        ll = va_arg(args, long long int);
        break;
      case LengthSpecifier::Size:
        goto Fail;
      }

      if (ll < 0) {
        tmpbuf.Write('-');
        if (ll == std::numeric_limits<long long>::min()) {
          ll  = -(ll + 1);
          ull = static_cast<unsigned long long>(ll) + 1;
        } else
          ull = static_cast<unsigned long long>(-ll);
      } else
        ull = ll;

      goto WriteNumber;
    }

    if (ch == 'u' || ch == 'b' || ch == 'B' || ch == 'o' || ch == 'x' ||
        ch == 'X') {
      switch (ls) {
      case LengthSpecifier::None:
        ull = va_arg(args, unsigned int);
        break;
      case LengthSpecifier::Half:
        ull = static_cast<unsigned short int>(va_arg(args, unsigned int));
        break;
      case LengthSpecifier::HalfHalf:
        ull = static_cast<unsigned char>(va_arg(args, unsigned int));
        break;
      case LengthSpecifier::Long:
        ull = va_arg(args, unsigned long int);
        break;
      case LengthSpecifier::LongLong:
        ull = va_arg(args, unsigned long long int);
        break;
      case LengthSpecifier::Size:
        ull = va_arg(args, size_t);
        break;
      }
    }

    if (ch == 'b' || ch == 'B') {
      is_capital = ch == 'B';
      base       = 2;
      tmpbuf.Write('0');
      tmpbuf.Write(ch);
      goto WriteNumber;
    }

    if (ch == 'o') {
      base = 8;
      tmpbuf.Write('0');
      goto WriteNumber;
    }

    if (ch == 'x' || ch == 'X') {
      is_capital = ch == 'X';
      base       = 16;
      tmpbuf.Write('0');
      tmpbuf.Write(ch);
      goto WriteNumber;
    }

    if (ch == 'p') {
      if (ls != LengthSpecifier::None)
        goto Fail;

      static_assert(sizeof(unsigned long long) >= sizeof(uintptr_t));
      ull  = reinterpret_cast<uintptr_t>(va_arg(args, void *));
      base = 16;
      tmpbuf.Write('0');
      tmpbuf.Write('x');
    }

  WriteNumber:

    if (ull == 0)
      tmpbuf.Write('0');
    else {
      // Note: tmpbuf capacity must be greater than max number of chars we write
      // so that we do not prematurely write tmpbuf.
      static_assert(TMP_BUF_SIZE >= sizeof(ull) * CHAR_BIT);

      tmpbuf.WriteBuf();

      auto chars = is_capital ? "0123456789ABCDEF" : "0123456789abcdef";

      while (ull) {
        unsigned char value = ull % base;
        ull /= base;
        tmpbuf.Write(chars[value]);
      }

      tmpbuf.Reverse();
    }

    goto Read;
  }

  // We got a bad specifier.

Fail: {
  result = -1;
  if (tmpbuf.m_tlen)
    tmpbuf.Write('\n');
  tmpbuf.Write("WARNING: Invalid format specifier '%");
  tmpbuf.Write(prev, fmt - prev);
  tmpbuf.Write("'\n");
}

End:
  tmpbuf.WriteBuf();

  Log.GetLog().m_lock.Release<IRQ::LockMode::IRQSave>();

  if (flush)
    Log.Flush();

  return !result ? tmpbuf.m_tlen : result;
}

int Print(const char *fmt, int flush, ...) {
  int i;
  va_list args;
  va_start(args, flush);
  i = Print(fmt, flush, args);
  va_end(args);
  return i;
}

int Print(const char *fmt, ...) {
  int i;
  va_list args;
  va_start(args, fmt);
  i = Print(fmt, true, args);
  va_end(args);
  return i;
}

int PrintLn(const char *fmt, ...) {
  int i, j;
  va_list args;
  va_start(args, fmt);
  i = Print(fmt, false, args);
  va_end(args);
  if (i < 0) {
    Log.Flush();
    return i;
  }
  j = Print("\n", true);
  if (j < 0)
    return j;
  if (i == std::numeric_limits<int>::max())
    return i;
  return j + i;
}

} // namespace IO
