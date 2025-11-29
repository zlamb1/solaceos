#pragma once

#include "common.h"
#include "smp/lock.hpp"
#include "smp/spinlock.hpp"

namespace IO {

class ConsoleDevice {
  friend class KernelConsole;

public:
  SMP::SpinLock &GetConsoleLock();

  virtual ~ConsoleDevice() = default;

  template <SMP::LockOperation LO> void Flush() {
    if constexpr (LO == SMP::LockOperation::Unlocked)
      Flush();
    else {
      auto guard = m_lock.AsGuard();
      Flush();
    }
  }

  template <SMP::LockOperation LO> void Write(const char *str, usize len) {
    if constexpr (LO == SMP::LockOperation::Unlocked)
      Write(str, len);
    else {
      auto guard = m_lock.AsGuard();
      Write(str, len);
    }
  }

protected:
  SMP::SpinLock m_lock;

  ConsoleDevice *m_next = nullptr;

  virtual void Flush() {}
  virtual void Write(const char *str, usize len) = 0;
};

} // namespace IO
