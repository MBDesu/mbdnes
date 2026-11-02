#pragma once
#ifndef MODULE_IN_MEMORY_REGISTER_H
#define MODULE_IN_MEMORY_REGISTER_H

#include "globals.h"

class InMemoryRegister {
protected:
  u8 m_value = 0;

public:
  InMemoryRegister() : m_value{0} {}
  ~InMemoryRegister() = default;

  virtual void onLoad() {};
  virtual u8 onRead() { return 0; };
  virtual void onWrite(u8 value) {};

  u8 getValue() const { return m_value; }

private:
};

#endif // MODULE_IN_MEMORY_REGISTER_H
