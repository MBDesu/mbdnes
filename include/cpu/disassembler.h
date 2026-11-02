#pragma once
#ifndef MODULE_CPU_DISASSEMBLER_H
#define MODULE_CPU_DISASSEMBLER_H

#include "cpu.h"
#include "globals.h"

u8 disassembleInstructionTo(char *buffer, u8 opcode, u16 operand,
                            u16 currentPc) noexcept;

void printDisassembly(CPULogFrame &frame, u8 inputSize, u16 input);

#endif // MODULE_CPU_DISASSEMBLER_H
