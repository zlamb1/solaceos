#pragma once

namespace IRQ {

enum class LockMode {
  None,
  IRQ,
  IRQSave,
};

/* Note: These functions must act as compiler barriers. */

int Save();
void Enable();
void Disable();
void Restore(int irq);

} // namespace IRQ
