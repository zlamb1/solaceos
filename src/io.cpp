#include "io.hpp"

namespace IO {

class DummyConsoleDevice : public ConsoleDevice {
public:
  virtual void WriteByte(char byte) override { (void) byte; }

  virtual void WriteBuffer(const char *buf, usize len) override {
    (void) buf;
    (void) len;
  }

  virtual void Flush() override {}
} dummyConsole;

IOConsole::IOConsole() { consoleDevice = &dummyConsole; }

void IOConsole::SetConsoleDevice(ConsoleDevice *consoleDevice) {
  this->consoleDevice =
      consoleDevice == nullptr ? &dummyConsole : consoleDevice;
}

IOConsole Log{};

} // namespace IO
