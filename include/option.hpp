#pragma once

#include <new>
#include <utility>

template <typename T> class Option {
public:
  Option() : isSome(false) {}
  Option(T _value) : isSome(true) { new (value) T(_value); }

  template <typename... Args> Option(Args &&...args) : isSome(true) {
    new (value) T(std::forward<Args>(args)...);
  }

#define UNWRAP(VALUE) reinterpret_cast<T *>(VALUE)

  ~Option() {
    if (isSome)
      UNWRAP(value)->~T();
  };

  static Option<T> None() { return Option<T>(); }

  template <typename S> static Option<S> None() { return Option<S>(); }

  static Option<T> Some(T value) { return Option<T>(value); }

  template <typename... Args> static Option<T> Some(Args &&...args) {
    return Option<T>(std::forward<Args>(args)...);
  }

  bool hasValue() const { return isSome; }

  T &unwrap() {
    // TODO: add kernel panic if !isSome
    return *UNWRAP(value);
  }

  T &unwrapOr(T &orValue) { return isSome ? *UNWRAP(value) : orValue; }

  T &unwrapUnchecked() { return *UNWRAP(value); }

#undef UNWRAP

protected:
  bool isSome;
  alignas(T) unsigned char value[sizeof(T)];
};