#pragma once
#ifndef MODULE_CPU_INSTRUCTIONS_H
#define MODULE_CPU_INSTRUCTIONS_H

#include "globals.h"

#include <functional>

class CPU; // forward declaration
struct Instruction {
  std::function<void(CPU &)> runNoArgs = nullptr;
  std::function<void(CPU &, u16)> runAddress = nullptr;
  std::function<void(CPU &, u8)> runValue = nullptr;
  void run(CPU &cpu) {
    if (runNoArgs)
      runNoArgs(cpu);
  }
  void run(CPU &cpu, u16 address) {
    if (runAddress)
      runAddress(cpu, address);
  }
  void run(CPU &cpu, u8 value) {
    if (runValue)
      runValue(cpu, value);
  }
};

namespace Instructions {

extern const Instruction INX;
extern const Instruction INC;
extern const Instruction ADC;
extern const Instruction ASL;
extern const Instruction ASLa;
extern const Instruction DEC;
extern const Instruction DEX;
extern const Instruction DEY;
extern const Instruction INY;
extern const Instruction LSR;
extern const Instruction LSRa;
extern const Instruction ROL;
extern const Instruction ROLa;
extern const Instruction ROR;
extern const Instruction RORa;
extern const Instruction SBC;
extern const Instruction CLC;
extern const Instruction CLD;
extern const Instruction CLI;
extern const Instruction CLV;
extern const Instruction LDA;
extern const Instruction LDX;
extern const Instruction LDY;
extern const Instruction PHA;
extern const Instruction PHP;
extern const Instruction PLA;
extern const Instruction PLP;
extern const Instruction SEC;
extern const Instruction SED;
extern const Instruction SEI;
extern const Instruction STA;
extern const Instruction STX;
extern const Instruction STY;
extern const Instruction TAX;
extern const Instruction TAY;
extern const Instruction TSX;
extern const Instruction TXA;
extern const Instruction TXS;
extern const Instruction TYA;
extern const Instruction BIT;
extern const Instruction CMP;
extern const Instruction CPX;
extern const Instruction CPY;
extern const Instruction AND;
extern const Instruction EOR;
extern const Instruction ORA;
extern const Instruction BCC;
extern const Instruction BCS;
extern const Instruction BEQ;
extern const Instruction BNE;
extern const Instruction BMI;
extern const Instruction BPL;
extern const Instruction BVC;
extern const Instruction BVS;
extern const Instruction JMP;
extern const Instruction JSR;
extern const Instruction RTI;
extern const Instruction RTS;
extern const Instruction BRK;
extern const Instruction NOP;
extern const Instruction LAX;
extern const Instruction SAX;
extern const Instruction SBC_UNOFFICIAL;
extern const Instruction DCP;
extern const Instruction ISB;
extern const Instruction SLO;
extern const Instruction RLA;
extern const Instruction SRE;
extern const Instruction RRA;
extern const Instruction AAC;
extern const Instruction ASR; // aka ALR
extern const Instruction ARR;
extern const Instruction ATX; // aka LXA
extern const Instruction AXS; // aka SBX
extern const Instruction SYA;
extern const Instruction SXA;
} // namespace Instructions

#endif // MODULE_CPU_INSTRUCTIONS_H
