#pragma once

#include "common.h"
#include "io/console.hpp"
#include "smp/spinlock.hpp"

namespace IO {

class KernelConsole {
public:
  void AddConsoleDevice(ConsoleDevice *console, bool writeLog = true);

  void Flush();

  void Write(const char *str);
  void Write(const char *str, usize len);

  inline void operator<<(const char *str) { Write(str); }

private:
  struct Log {
  public:
    inline static char m_init[512];

    SMP::SpinLock m_lock;
    char *m_log = m_init;

    u32 m_head = 0, m_tail = 0, m_first = 0, m_len = sizeof(m_init);

    void Write(char ch);
  } m_klog{};

  SMP::SpinLock m_lock;
  ConsoleDevice *m_consoles = nullptr;
};

extern KernelConsole Log;

} // namespace IO