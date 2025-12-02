#pragma once

#include "arch.hpp"
#include "irq.hpp"

#include "atomic.hpp"

namespace SMP {

class SpinLock {
public:
  template <IRQ::LockMode LM> class Guard {
  public:
    inline Guard(SpinLock &m_lock) : m_lock(m_lock) { m_lock.Acquire<LM>(); }

    inline ~Guard() { m_lock.Release<LM>(); }

    Guard(const Guard &)            = delete;
    Guard &operator=(const Guard &) = delete;

    Guard(Guard &&)            = delete;
    Guard &operator=(Guard &&) = delete;

  private:
    SpinLock &m_lock;
  };

  static constexpr int Unlocked = 0;
  static constexpr int Locked   = 1;

  SpinLock() : m_lock(Unlocked), m_irq(0) {};

  template <IRQ::LockMode LM> void Acquire();

  template <IRQ::LockMode LM> bool TryAcquire();

  template <IRQ::LockMode LM> void Release();

  template <IRQ::LockMode LM> Guard<LM> AsGuard() { return Guard<LM>(*this); }

  Guard<IRQ::LockMode::IRQSave> AsGuard() {
    return Guard<IRQ::LockMode::IRQSave>(*this);
  }

protected:
  Atomic<int> m_lock;
  int m_irq;
};

template <IRQ::LockMode LM> void SpinLock::Acquire() {
  int expected = Unlocked, irq;

  if constexpr (LM == IRQ::LockMode::IRQ)
    IRQ::Disable();

  if constexpr (LM == IRQ::LockMode::IRQSave) {
    irq = IRQ::Save();
    IRQ::Disable();
  }

  while (!m_lock.CompareExchangeWeak<MemoryOrder::Acquire>(expected, Locked)) {
    while (m_lock.Load<MemoryOrder::Relaxed>() == Locked) {
      Arch::BusyWaiting();
      continue;
    }
    expected = Unlocked;
  }

  if constexpr (LM == IRQ::LockMode::IRQSave)
    m_irq = irq;
}

template <IRQ::LockMode LM> bool SpinLock::TryAcquire() {
  int expected = Unlocked, irq;

  if constexpr (LM == IRQ::LockMode::IRQ)
    IRQ::Disable();

  if constexpr (LM == IRQ::LockMode::IRQSave) {
    irq = IRQ::Save();
    IRQ::Disable();
  }

  if (m_lock.CompareExchangeStrong<MemoryOrder::Acquire>(expected, Locked)) {
    if constexpr (LM == IRQ::LockMode::IRQSave)
      m_irq = irq;

    return true;
  }

  if constexpr (LM == IRQ::LockMode::IRQ)
    IRQ::Enable();

  if constexpr (LM == IRQ::LockMode::IRQSave)
    IRQ::Restore(irq);

  return false;
}

template <IRQ::LockMode LM> void SpinLock::Release() {
  int irq;

  if constexpr (LM == IRQ::LockMode::IRQSave) {
    irq   = m_irq;
    m_irq = 0;
  }

  m_lock.Store<MemoryOrder::Release>(Unlocked);

  if constexpr (LM == IRQ::LockMode::IRQ)
    IRQ::Enable();

  if constexpr (LM == IRQ::LockMode::IRQSave)
    IRQ::Restore(irq);
}

} // namespace SMP