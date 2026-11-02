#pragma once
#ifndef MODULE_CPU_OPERATIONS_H
#define MODULE_CPU_OPERATIONS_H

#include "addressing_modes.h"
#include "instructions.h"

struct Operation {
  u8 id;
  Instruction instruction;
  u8 cycles;
  const AddressingMode *addressingMode;
  bool hasPageCrossPenalty = false;
};

extern const std::array<Operation, 256> OPERATIONS;

#endif // MODULE_CPU_OPERATIONS_H
