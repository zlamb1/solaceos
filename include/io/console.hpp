#pragma once

#include "common.h"
#include "smp/spinlock.hpp"

namespace IO {

class ConsoleDevice {
  friend class KernelConsole;

public:
  SMP::SpinLock &GetConsoleLock();

  virtual ~ConsoleDevice() = default;

  void Flush() {
    auto guard = m_lock.AsGuard();
    FlushUnlocked();
  }

  virtual void FlushUnlocked() {}

  inline void Write(const char *str, usize len) {
    auto guard = m_lock.AsGuard();
    WriteUnlocked(str, len);
  }

  virtual void WriteUnlocked(const char *str, usize len) = 0;

protected:
  SMP::SpinLock m_lock;

  ConsoleDevice *m_next = nullptr;
};

} // namespace IO
