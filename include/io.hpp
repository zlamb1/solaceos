#pragma once

#include "con/con.hpp"

namespace IO {

using ostream = AbstractOutputStream;

class IOConsole {
public:
  IOConsole();

  void SetConsoleDevice(ConsoleDevice *consoleDevice);

  template <typename T> ostream &operator<<(T &value) {
    return *consoleDevice << value;
  }

  inline ostream &endl() { return consoleDevice->endl(); }

protected:
  ConsoleDevice *consoleDevice;
};

extern IOConsole Log;

static inline ostream &endl() { return Log.endl(); }

} // namespace IO