#pragma once
#ifndef MODULE_LIB_BYTE_H
#define MODULE_LIB_BYTE_H

#include "../globals.h"

template <typename T> constexpr T MAKE_MASK(const u8 start, const u8 size) {
  return ((T(1) << size) - 1) << start;
}

template <typename T>
constexpr T BITS(const T value, const u8 start, const u8 size) {
  return (value >> start) & MAKE_MASK<T>(0, size);
}

template <typename T> constexpr T BIT(const T value, const u8 pos) {
  return BITS(value, pos, 1);
}

template <typename T, u8 bit> struct Bit {
  const T &source;
  operator bool() const { return BIT(source, bit); }
};

template <typename T, u8 start, u8 size> struct Bits {
  const T &source;
  operator T() const { return BITS(source, start, size); };
};

#endif // MODULE_LIB_BYTE_H
