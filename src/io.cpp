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
  if (m_tail == m_head && ++m_head == m_len) {
    m_head = 0;
  }
}

} // namespace IO
