#pragma once

#include "common.h"
#include "io/console.hpp"
#include "smp/lock.hpp"
#include "smp/spinlock.hpp"

namespace IO {

class KernelConsole {
public:
  struct Log {
  public:
    inline static char m_init[512];

    SMP::SpinLock m_lock;
    char *m_log = m_init;

    u32 m_head = 0, m_tail = 0, m_first = 0, m_len = sizeof(m_init);

    void Write(char ch);
    void Write(const char *str, usize len);
  };

  inline Log &GetLog() { return m_klog; }

  void AddConsoleDevice(ConsoleDevice *console, bool writeLog = true);

  void Flush();

  void Write(const char *str, bool flush = true);
  void Write(const char *str, usize len, bool flush = true);

  inline void operator<<(const char *str) { Write(str); }

private:
  Log m_klog;

  SMP::SpinLock m_lock;
  ConsoleDevice *m_consoles = nullptr;
};

extern KernelConsole Log;

int Print(const char *fmt, ...);
int PrintLn(const char *fmt, ...);

} // namespace IO