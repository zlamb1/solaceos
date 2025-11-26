#pragma once

#include "con/con.hpp"

namespace IO {

using ostream = AbstractOutputStream;

static auto EndLine = ostream::endl;

class IOConsole {
public:
  IOConsole();

  void SetConsoleDevice(ConsoleDevice *consoleDevice);

  template <typename T> ostream &operator<<(T &value) {
    return *consoleDevice << value;
  }

protected:
  ConsoleDevice *consoleDevice;
};

extern IOConsole Log;

} // namespace IO