#pragma once

#include "common.h"

namespace IO {

class AbstractOutputStream {
public:
  virtual AbstractOutputStream &operator<<(bool b)          = 0;
  virtual AbstractOutputStream &operator<<(char ch)         = 0;
  virtual AbstractOutputStream &operator<<(const char *str) = 0;
  virtual AbstractOutputStream &operator<<(i32 n)           = 0;
  virtual AbstractOutputStream &operator<<(u32 n)           = 0;
  virtual AbstractOutputStream &operator<<(i64 n)           = 0;
  virtual AbstractOutputStream &operator<<(u64 n)           = 0;
  virtual AbstractOutputStream &endl()                      = 0;
};

template <typename T> class OutputStream : public AbstractOutputStream {
  using AOS = AbstractOutputStream;

public:
  AOS &operator<<(bool b) override { return impl().Write(b); }
  AOS &operator<<(char ch) override { return impl().Write(ch); }
  AOS &operator<<(const char *str) override { return impl().Write(str); }
  AOS &operator<<(i32 n) override { return impl().Write(n); }
  AOS &operator<<(u32 n) override { return impl().Write(n); }
  AOS &operator<<(i64 n) override { return impl().Write(n); }
  AOS &operator<<(u64 n) override { return impl().Write(n); }

private:
  T &impl() { return static_cast<T &>(*this); }
};

class BasicOutputStream : public OutputStream<BasicOutputStream> {
  friend class OutputStream<BasicOutputStream>;
  using AOS = AbstractOutputStream;

public:
  virtual void WriteByte(char byte)                    = 0;
  virtual void WriteBuffer(const char *buf, usize len) = 0;
  virtual void Flush()                                 = 0;

  virtual AOS &endl() override {
    WriteByte('\n');
    Flush();
    return *this;
  }

private:
  AOS &Write(bool b) {
    WriteByte(b ? '1' : '0');
    return *this;
  }

  AOS &Write(char ch) {
    WriteByte(ch);
    return *this;
  }

  AOS &Write(const char *str) {
    if (str == nullptr)
      WriteBuffer("<null>", 6);
    else
      for (; str[0]; ++str)
        WriteByte(str[0]);
    return *this;
  }

  AOS &Write(i32 n) {
    if (n < 0) {
      if (n == INT32_MIN) {
        WriteBuffer("-2147483648", 11);
        return *this;
      }
      WriteByte('-');
      return operator<<(static_cast<u32>(-n));
    }
    return operator<<(static_cast<u32>(n));
  }

  AOS &Write(u32 n) {
    u32 mod;
    u8 len = 0;
    char buf[10];

    if (!n) {
      WriteByte('0');
      goto EndWrite;
    }

    while (n) {
      mod = n % 10;
      n /= 10;
      buf[len++] = '0' + mod;
    }

    for (int i = static_cast<int>(len) - 1; i >= 0; --i)
      WriteByte(buf[i]);

  EndWrite:
    return *this;
  }

  AOS &Write(i64 n) {
    if (n < 0) {
      if (n == INT64_MIN) {
        WriteBuffer("-9223372036854775808", 20);
        return *this;
      }
      WriteByte('-');
      return operator<<(static_cast<u64>(-n));
    }
    return operator<<(static_cast<u64>(n));
  }

  AOS &Write(u64 n) {
    u64 mod;
    u8 len = 0;
    char buf[20];

    if (!n) {
      WriteByte('0');
      goto EndWrite;
    }

    while (n) {
      mod = n % 10;
      n /= 10;
      buf[len++] = '0' + mod;
    }

    for (int i = static_cast<int>(len) - 1; i >= 0; --i)
      WriteByte(buf[i]);

  EndWrite:
    return *this;
  }
};

// IMPLEMENT THIS
class ConsoleDevice : public BasicOutputStream {};

} // namespace IO
