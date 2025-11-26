#pragma once

#include "new.hpp"

template <typename T> class Option {
public:
  Option() : isSome(false) {}
  Option(T _value) : isSome(true) { new (value) T(_value); }

#define UNWRAP(VALUE) reinterpret_cast<T *>(VALUE)

  ~Option() {
    if (isSome)
      UNWRAP(value)->~T();
  };

  static Option<T> None() { return Option(); }
  template <typename S> static Option<S> Some(S value) {
    return Option<S>(value);
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

template <typename S> static Option<S> Some(S value) {
  return Option<S>(value);
}