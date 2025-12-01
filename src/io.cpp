#include <climits>
#include <cstdarg>
#include <limits>

#include "io.hpp"
#include "smp/lock.hpp"

namespace IO {

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

template <unsigned char N> struct TmpBuf {
  char buf[N];
  int len = 0;

  static_assert(N > 0);

  void Write(char ch) {
    buf[len++] = ch;
    if (len == N) {
      Log.Write(buf, len, false);
      len = 0;
    }
  }

  void Write(const char *str) {
    for (; *str; ++str)
      Write(*str);
  }

  void Reverse() {
    for (int i = 0; i < len >> 1; ++i) {
      char tmp         = buf[i];
      buf[i]           = buf[len - 1 - i];
      buf[len - 1 - i] = tmp;
    }
  }

  void WriteBuf() {
    if (len) {
      Log.Write(buf, len, false);
      len = 0;
    }
  }
};

#define TMP_BUF_SIZE 128

void Print(const char *fmt, int flush, va_list args) {
  char ch;
  TmpBuf<TMP_BUF_SIZE> tmpbuf;

  // FIXME: change this to acquire klog lock to ensure atomic prints

Read:
  ch = *fmt++;

  if (ch != '%') {
    if (!ch)
      goto End;

    tmpbuf.Write(ch);
    goto Read;
  }

  ch = *fmt++;

  if (ch == '%') {
    tmpbuf.Write('%');
    goto Read;
  }

  if (ch == 'c') {
    tmpbuf.Write(va_arg(args, int));
    goto Read;
  }

  if (ch == 's') {
    tmpbuf.Write(va_arg(args, const char *));
    goto Read;
  }

  if (ch == 'd' || ch == 'i' || ch == 'u' || ch == 'b' || ch == 'B' ||
      ch == 'o' || ch == 'x' || ch == 'X') {
    bool is_capital    = false;
    unsigned char base = 10;

    long long ll;
    unsigned long long ull;

    if (ch == 'd' || ch == 'i') {
      ll = va_arg(args, int);

      if (ll < 0) {
        tmpbuf.Write('-');
        if (ll == std::numeric_limits<long long>::min()) {
          ll  = -(ll + 1);
          ull = static_cast<unsigned long long>(ll) + 1;
        } else
          ull = static_cast<unsigned long long>(-ll);
      } else
        ull = ll;
    }

    if (ch == 'u')
      ull = va_arg(args, unsigned int);

    if (ch == 'b' || ch == 'B') {
      is_capital = ch == 'B';
      base       = 2;
      ull        = va_arg(args, unsigned int);
      tmpbuf.Write('0');
      tmpbuf.Write(ch);
    }

    if (ch == 'o') {
      base = 8;
      ull  = va_arg(args, unsigned int);
      tmpbuf.Write('0');
    }

    if (ch == 'x' || ch == 'X') {
      is_capital = ch == 'X';
      base       = 16;
      ull        = va_arg(args, unsigned int);
      tmpbuf.Write('0');
      tmpbuf.Write(ch);
    }

    if (ull == 0)
      tmpbuf.Write('0');
    else {
      // Note: tmpbuf capacity must be greater than max number of chars we write
      // so that we do not prematurely write tmpbuf
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

  // invalid specifier
  // FIXME: emit warning

End:
  tmpbuf.WriteBuf();
  if (flush)
    Log.Flush();
}

void Print(const char *fmt, int flush, ...) {
  va_list args;
  va_start(args, flush);
  Print(fmt, flush, args);
  va_end(args);
}

void Print(const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);
  Print(fmt, true, args);
  va_end(args);
}

void PrintLn(const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);
  Print(fmt, false, args);
  Print("\n", true);
  va_end(args);
}

} // namespace IO
