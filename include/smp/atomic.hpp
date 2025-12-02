#pragma once

enum class MemoryOrder : int {
  Relaxed,
  Acquire,
  Release,
  AcqRel,
  SeqCst,
};

template <typename T> class Atomic {
public:
  Atomic(T value) : m_value(value) {}

  template <MemoryOrder MO> T Load() const;

  template <MemoryOrder MO> void Store(T desired);

  template <MemoryOrder MO> bool CompareExchangeWeak(T &expected, T desired);

  template <MemoryOrder MO> bool CompareExchangeStrong(T &expected, T desired);

protected:
  T m_value;
};
