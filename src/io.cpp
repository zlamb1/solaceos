#include "io.hpp"

namespace IO {

KernelConsole Log = KernelConsole();

void KernelConsole::AddConsoleDevice(ConsoleDevice *console) {
  auto guard      = m_lock.AsGuard();
  console->m_next = m_consoles;
  m_consoles      = console;
}

void KernelConsole::Flush() {
  auto klog_guard     = m_klog.m_lock.AsGuard();
  auto consoles_guard = m_lock.AsGuard();
  auto console        = m_consoles;

  if (m_klog.m_first <= m_klog.m_tail) {
    usize len   = m_klog.m_tail - m_klog.m_first;
    char *first = m_klog.m_log + m_klog.m_first;

    while (console != nullptr) {
      console->Write(first, len);
      console = console->m_next;
    }
  } else {
    usize len   = m_klog.m_len - m_klog.m_first;
    char *first = m_klog.m_log + m_klog.m_first;

    while (console != nullptr) {
      auto console_guard = console->m_lock.AsGuard();
      console->WriteUnlocked(first, len);
      console->WriteUnlocked(m_klog.m_log, m_klog.m_tail);
      console->FlushUnlocked();
      console = console->m_next;
    }
  }

  m_klog.m_first = m_klog.m_tail;
}

void KernelConsole::Write(const char *str) {
  if (!m_klog.m_len)
    return;

  {
    auto guard = m_klog.m_lock.AsGuard();

    for (; str[0]; ++str)
      m_klog.Write(str[0]);
  }

  Flush();
}

void KernelConsole::Write(const char *str, usize len) {
  if (!len || !m_klog.m_len)
    return;

  {
    auto guard = m_klog.m_lock.AsGuard();

    for (usize i = 0; i < len; ++i)
      m_klog.Write(str[i]);
  }

  Flush();
}

void KernelConsole::Log::Write(char ch) {
  m_log[m_tail++] = ch;
  if (m_tail == m_len)
    m_tail = 0;
}

} // namespace IO
