#include "cpu/instructions.h"
#include "cpu/cpu.h"

namespace Instructions {

const Instruction INX = {.runNoArgs = [](CPU &cpu) {
  cpu.m_x.increment();
  cpu.m_flags.updateNegativeAndZero(cpu.m_x.getContent());
}};

const Instruction INC = {.runAddress = [](CPU &cpu, u16 address) -> void {
  const auto newValue = cpu.memory().read(address) + 1;
  cpu.memory().write(address, newValue);
  cpu.m_flags.updateNegativeAndZero(newValue);
}};

const Instruction ADC = {.runValue = [](CPU &cpu, u8 value) -> void {
  const u16 oldValue = cpu.m_a.getContent();
  const u16 carry = cpu.m_flags.m_c ? 1 : 0;
  const u16 sum = oldValue + value + carry;

  const u8 newValue = static_cast<u8>(sum & 0xFF);

  cpu.m_flags.m_v = (~(oldValue ^ value) & (oldValue ^ sum) & 0x80) != 0;
  cpu.m_flags.m_c = sum > 0xff;

  cpu.m_a.setContent(newValue);
  cpu.m_flags.updateNegativeAndZero(newValue);
}};

const Instruction ASL = {.runAddress = [](CPU &cpu, u16 address) {
  const auto value = cpu.memory().read(address);
  cpu.m_flags.m_c = (value >> 7) & 1;
  const u8 newValue = (value << 1) & 0xff;
  cpu.memory().write(address, newValue);
  cpu.m_flags.updateNegativeAndZero(newValue);
}};

const Instruction ASLa = {.runNoArgs = [](CPU &cpu) {
  const auto value = cpu.m_a.getContent();
  cpu.m_flags.m_c = (value >> 7) & 1;
  const u8 newValue = value << 1;
  cpu.m_a.setContent(newValue);
  cpu.m_flags.updateNegativeAndZero(newValue);
}};

const Instruction DEC = {.runAddress = [](CPU &cpu, u16 address) {
  const auto value = cpu.memory().read(address);
  const u8 newValue = value - 1;
  cpu.memory().write(address, newValue);
  cpu.m_flags.updateNegativeAndZero(newValue);
}};

const Instruction DEX = {.runNoArgs = [](CPU &cpu) {
  const auto value = cpu.m_x.getContent();
  const u8 newValue = value - 1;
  cpu.m_x.setContent(newValue);
  cpu.m_flags.updateNegativeAndZero(newValue);
}};

const Instruction DEY = {.runNoArgs = [](CPU &cpu) {
  const auto value = cpu.m_y.getContent();
  const u8 newValue = (value - 1) & 0xff;
  cpu.m_y.setContent(newValue);
  cpu.m_flags.updateNegativeAndZero(newValue);
}};

const Instruction INY = {.runNoArgs = [](CPU &cpu) {
  const auto value = cpu.m_y.getContent();
  const u8 newValue = (value + 1) & 0xff;
  cpu.m_y.setContent(newValue);
  cpu.m_flags.updateNegativeAndZero(newValue);
}};

const Instruction LSR = {.runAddress = [](CPU &cpu, u16 address) {
  const auto value = cpu.memory().read(address);
  cpu.m_flags.m_c = value & 1;
  const u8 newValue = (value >> 1) & 0xff;
  cpu.memory().write(address, newValue);
  cpu.m_flags.updateNegativeAndZero(newValue);
}};

const Instruction LSRa = {.runNoArgs = [](CPU &cpu) {
  const auto value = cpu.m_a.getContent();
  cpu.m_flags.m_c = value & 1;
  const u8 newValue = (value >> 1) & 0xff;
  cpu.m_a.setContent(newValue);
  cpu.m_flags.updateNegativeAndZero(newValue);
}};

const Instruction ROL = {.runAddress = [](CPU &cpu, u16 address) {
  const auto value = cpu.memory().read(address);
  const bool bit0 = cpu.m_flags.m_c;
  cpu.m_flags.m_c = (value >> 7) & 1;
  const u8 newValue = ((value << 1) | (bit0 & 1)) & 0xff;
  cpu.memory().write(address, newValue);
  cpu.m_flags.updateNegativeAndZero(newValue);
}};

const Instruction ROLa = {.runNoArgs = [](CPU &cpu) {
  const auto value = cpu.m_a.getContent();
  const bool bit0 = cpu.m_flags.m_c;
  cpu.m_flags.m_c = (value >> 7) & 1;
  const u8 newValue = ((value << 1) | (bit0 & 1)) & 0xff;
  cpu.m_a.setContent(newValue);
  cpu.m_flags.updateNegativeAndZero(newValue);
}};

const Instruction ROR = {.runAddress = [](CPU &cpu, u16 address) {
  const auto value = cpu.memory().read(address);
  const bool bit7 = cpu.m_flags.m_c;
  cpu.m_flags.m_c = value & 1;
  const u8 newValue = ((value >> 1) & 0x7f) | (bit7 << 7);
  cpu.memory().write(address, newValue);
  cpu.m_flags.updateNegativeAndZero(newValue);
}};

const Instruction RORa = {.runNoArgs = [](CPU &cpu) {
  const auto value = cpu.m_a.getContent();
  const bool bit7 = cpu.m_flags.m_c;
  cpu.m_flags.m_c = value & 1;
  const u8 newValue = ((value >> 1) & 0x7f) | (bit7 << 7);
  cpu.m_a.setContent(newValue);
  cpu.m_flags.updateNegativeAndZero(newValue);
}};

const Instruction SBC = {.runValue = [](CPU &cpu, u8 value) {
  const u8 oldValue = cpu.m_a.getContent();
  const u8 borrow = cpu.m_flags.m_c ? 0 : 1;
  const int intermediateResult =
      static_cast<int>(oldValue) - static_cast<int>(value) - borrow;
  const u8 newValue = static_cast<u8>(intermediateResult & 0xff);
  cpu.m_flags.m_v =
      ((oldValue ^ value) & 0x80) && ((oldValue ^ newValue) & 0x80);
  cpu.m_flags.m_c = intermediateResult >= 0;

  cpu.m_a.setContent(newValue);
  cpu.m_flags.updateNegativeAndZero(newValue);
}};

const Instruction CLC = {
    .runNoArgs = [](CPU &cpu) { cpu.m_flags.m_c = false; }};

const Instruction CLD = {
    .runNoArgs = [](CPU &cpu) { cpu.m_flags.m_d = false; }};

const Instruction CLI = {
    .runNoArgs = [](CPU &cpu) { cpu.m_flags.m_i = false; }};

const Instruction CLV = {
    .runNoArgs = [](CPU &cpu) { cpu.m_flags.m_v = false; }};

const Instruction LDA = {.runValue = [](CPU &cpu, u8 value) {
  cpu.m_a.setContent(value);
  cpu.m_flags.updateNegativeAndZero(value);
}};

const Instruction LDX = {.runValue = [](CPU &cpu, u8 value) {
  cpu.m_x.setContent(value);
  cpu.m_flags.updateNegativeAndZero(value);
}};

const Instruction LDY = {.runValue = [](CPU &cpu, u8 value) {
  cpu.m_y.setContent(value);
  cpu.m_flags.updateNegativeAndZero(value);
}};

const Instruction PHA = {
    .runNoArgs = [](CPU &cpu) { cpu.m_stack.push(cpu.m_a.getContent()); }};

const Instruction PHP = {.runNoArgs = [](CPU &cpu) {
  const auto flags = cpu.m_flags.getValue() | 0x30;
  cpu.m_stack.push(flags);
}};

const Instruction PLA = {.runNoArgs = [](CPU &cpu) {
  const auto value = cpu.m_stack.pop();
  cpu.m_a.setContent(value);
  cpu.m_flags.updateNegativeAndZero(value);
}};

const Instruction PLP = {.runNoArgs = [](CPU &cpu) {
  cpu.m_flags.setValue((cpu.m_stack.pop() & 0xEF) | 0x20);
}};

const Instruction SEC = {.runNoArgs = [](CPU &cpu) { cpu.m_flags.m_c = true; }};
const Instruction SED = {.runNoArgs = [](CPU &cpu) { cpu.m_flags.m_d = true; }};
const Instruction SEI = {.runNoArgs = [](CPU &cpu) { cpu.m_flags.m_i = true; }};

const Instruction STA = {.runAddress = [](CPU &cpu, u16 address) {
  cpu.memory().write(address, cpu.m_a.getContent());
}};

const Instruction STX = {.runAddress = [](CPU &cpu, u16 address) {
  cpu.memory().write(address, cpu.m_x.getContent());
}};

const Instruction STY = {.runAddress = [](CPU &cpu, u16 address) {
  cpu.memory().write(address, cpu.m_y.getContent());
}};

const Instruction TAX = {.runNoArgs = [](CPU &cpu) {
  const auto value = cpu.m_a.getContent();
  cpu.m_x.setContent(value);
  cpu.m_flags.updateNegativeAndZero(value);
}};

const Instruction TAY = {.runNoArgs = [](CPU &cpu) {
  const auto value = cpu.m_a.getContent();
  cpu.m_y.setContent(value);
  cpu.m_flags.updateNegativeAndZero(value);
}};

const Instruction TSX = {.runNoArgs = [](CPU &cpu) {
  const auto value = cpu.m_sp.getContent();
  cpu.m_x.setContent(value);
  cpu.m_flags.updateNegativeAndZero(value);
}};

const Instruction TXA = {.runNoArgs = [](CPU &cpu) {
  const auto value = cpu.m_x.getContent();
  cpu.m_a.setContent(value);
  cpu.m_flags.updateNegativeAndZero(value);
}};

const Instruction TXS = {.runNoArgs = [](CPU &cpu) {
  const auto value = cpu.m_x.getContent();
  cpu.m_sp.setContent(value);
}};

const Instruction TYA = {.runNoArgs = [](CPU &cpu) {
  const auto value = cpu.m_y.getContent();
  cpu.m_a.setContent(value);
  cpu.m_flags.updateNegativeAndZero(value);
}};

const Instruction BIT = {.runValue = [](CPU &cpu, u8 value) {
  cpu.m_flags.m_z = ((cpu.m_a.getContent() & value) == 0);
  cpu.m_flags.m_n = (value & 0x80) != 0;
  cpu.m_flags.m_v = (value & 0x40) != 0;
}};

const Instruction CMP = {.runValue = [](CPU &cpu, u8 value) {
  const auto a = cpu.m_a.getContent();
  cpu.m_flags.m_z = a == value;
  cpu.m_flags.m_n = ((a - value) & 0x80) != 0;
  cpu.m_flags.m_c = a >= value;
}};

const Instruction CPX = {.runValue = [](CPU &cpu, u8 value) {
  const auto x = cpu.m_x.getContent();
  cpu.m_flags.m_z = x == value;
  cpu.m_flags.m_n = ((x - value) & 0x80) != 0;
  cpu.m_flags.m_c = x >= value;
}};

const Instruction CPY = {.runValue = [](CPU &cpu, u8 value) {
  const auto y = cpu.m_y.getContent();
  cpu.m_flags.m_z = y == value;
  cpu.m_flags.m_n = ((y - value) & 0x80) != 0;
  cpu.m_flags.m_c = y >= value;
}};

const Instruction AND = {.runValue = [](CPU &cpu, u8 value) {
  cpu.m_a.setContent(cpu.m_a.getContent() & value);
  cpu.m_flags.updateNegativeAndZero(cpu.m_a.getContent());
}};

const Instruction EOR = {.runValue = [](CPU &cpu, u8 value) {
  cpu.m_a.setContent(cpu.m_a.getContent() ^ value);
  cpu.m_flags.updateNegativeAndZero(cpu.m_a.getContent());
}};

const Instruction ORA = {.runValue = [](CPU &cpu, u8 value) {
  cpu.m_a.setContent(cpu.m_a.getContent() | value);
  cpu.m_flags.updateNegativeAndZero(cpu.m_a.getContent());
}};

const Instruction BCC = {.runAddress = [](CPU &cpu, u16 address) {
  if (!cpu.m_flags.m_c) {
    const auto postFetchPC = cpu.m_pc.getContent();
    cpu.m_extraCycles += 1;

    if ((postFetchPC & 0xff00) != (address & 0xff00)) {
      cpu.m_extraCycles += 1;
    }
    cpu.m_pc.setContent(address);
  }
}};

const Instruction BCS = {.runAddress = [](CPU &cpu, u16 address) {
  if (cpu.m_flags.m_c) {
    const auto postFetchPC = cpu.m_pc.getContent();
    cpu.m_extraCycles += 1;
    if ((postFetchPC & 0xff00) != (address & 0xff00)) {
      cpu.m_extraCycles += 1;
    }
    cpu.m_pc.setContent(address);
  }
}};

const Instruction BEQ = {.runAddress = [](CPU &cpu, u16 address) {
  if (cpu.m_flags.m_z) {
    const auto postFetchPC = cpu.m_pc.getContent();
    cpu.m_extraCycles += 1;
    if ((postFetchPC & 0xff00) != (address & 0xff00)) {
      cpu.m_extraCycles += 1;
    }
    cpu.m_pc.setContent(address);
  }
}};

const Instruction BNE = {.runAddress = [](CPU &cpu, u16 address) {
  if (!cpu.m_flags.m_z) {
    const auto postFetchPC = cpu.m_pc.getContent();
    cpu.m_extraCycles += 1;
    if ((postFetchPC & 0xff00) != (address & 0xff00)) {
      cpu.m_extraCycles += 1;
    }
    cpu.m_pc.setContent(address);
  }
}};

const Instruction BMI = {.runAddress = [](CPU &cpu, u16 address) {
  if (cpu.m_flags.m_n) {
    const auto postFetchPC = cpu.m_pc.getContent();
    cpu.m_extraCycles += 1;
    if ((postFetchPC & 0xff00) != (address & 0xff00)) {
      cpu.m_extraCycles += 1;
    }
    cpu.m_pc.setContent(address);
  }
}};

const Instruction BPL = {.runAddress = [](CPU &cpu, u16 address) {
  if (!cpu.m_flags.m_n) {
    const auto postFetchPC = cpu.m_pc.getContent();
    cpu.m_extraCycles += 1;
    if ((postFetchPC & 0xff00) != (address & 0xff00)) {
      cpu.m_extraCycles += 1;
    }
    cpu.m_pc.setContent(address);
  }
}};

const Instruction BVC = {.runAddress = [](CPU &cpu, u16 address) {
  if (!cpu.m_flags.m_v) {
    const auto postFetchPC = cpu.m_pc.getContent();
    cpu.m_extraCycles += 1;
    if ((postFetchPC & 0xff00) != (address & 0xff00)) {
      cpu.m_extraCycles += 1;
    }
    cpu.m_pc.setContent(address);
  }
}};

const Instruction BVS = {.runAddress = [](CPU &cpu, u16 address) {
  if (cpu.m_flags.m_v) {
    const auto postFetchPC = cpu.m_pc.getContent();
    cpu.m_extraCycles += 1;
    if ((postFetchPC & 0xff00) != (address & 0xff00)) {
      cpu.m_extraCycles += 1;
    }
    cpu.m_pc.setContent(address);
  }
}};

const Instruction JMP = {
    .runAddress = [](CPU &cpu, u16 address) { cpu.m_pc.setContent(address); }};

const Instruction JSR = {.runAddress = [](CPU &cpu, u16 address) {
  cpu.m_stack.push16(cpu.m_pc.getContent() - 1);
  cpu.m_pc.setContent(address);
}};

const Instruction RTI = {.runNoArgs = [](CPU &cpu) {
  Instructions::PLP.runNoArgs(cpu);
  cpu.m_pc.setContent(cpu.m_stack.pop16());
}};

const Instruction RTS = {.runNoArgs = [](CPU &cpu) {
  cpu.m_pc.setContent(cpu.m_stack.pop16() + 1);
}};

const Instruction BRK = {.runNoArgs = [](CPU &cpu) {
  cpu.m_pc.increment();
  cpu.interrupt(INTERRUPTS[Interrupts::BRK], true);
}};

const Instruction NOP = {};

// ----------------- illegal opcodes -----------------
const Instruction LAX = {.runValue = [](CPU &cpu, u8 value) {
  cpu.m_a.setContent(value);
  cpu.m_x.setContent(value);
  cpu.m_flags.updateNegativeAndZero(value);
}};

const Instruction SAX = {.runAddress = [](CPU &cpu, u16 address) {
  u8 result = cpu.m_a.getContent() & cpu.m_x.getContent();
  cpu.memory().write(address, result);
}};

const Instruction DCP = {.runAddress = [](CPU &cpu, u16 address) {
  u8 newValue = cpu.memory().read(address) - 1;
  cpu.memory().write(address, newValue);

  const auto a = cpu.m_a.getContent();
  cpu.m_flags.m_z = a == newValue;
  cpu.m_flags.m_n = ((a - newValue) & 0x80) != 0;
  cpu.m_flags.m_c = a >= newValue;
}};

const Instruction ISB = {.runAddress = [](CPU &cpu, u16 address) {
  u8 newValue = cpu.memory().read(address) + 1;
  cpu.memory().write(address, newValue);

  const u8 oldValue = cpu.m_a.getContent();
  const u8 borrow = cpu.m_flags.m_c ? 0 : 1;
  const int intermediateResult =
      static_cast<int>(oldValue) - static_cast<int>(newValue) - borrow;
  const u8 finalA = static_cast<u8>(intermediateResult & 0xff);

  cpu.m_flags.m_v =
      ((oldValue ^ newValue) & 0x80) && ((oldValue ^ finalA) & 0x80);
  cpu.m_flags.m_c = intermediateResult >= 0;
  cpu.m_a.setContent(finalA);
  cpu.m_flags.updateNegativeAndZero(finalA);
}};

const Instruction SLO = {.runAddress = [](CPU &cpu, u16 address) {
  u8 value = cpu.memory().read(address);
  cpu.m_flags.m_c = (value >> 7) & 1;
  u8 newValue = (value << 1) & 0xff;
  cpu.memory().write(address, newValue);

  cpu.m_a.setContent(cpu.m_a.getContent() | newValue);
  cpu.m_flags.updateNegativeAndZero(cpu.m_a.getContent());
}};

const Instruction RLA = {.runAddress = [](CPU &cpu, u16 address) {
  u8 value = cpu.memory().read(address);
  bool bit0 = cpu.m_flags.m_c;
  cpu.m_flags.m_c = (value >> 7) & 1;
  u8 newValue = ((value << 1) | (bit0 & 1)) & 0xff;
  cpu.memory().write(address, newValue);

  cpu.m_a.setContent(cpu.m_a.getContent() & newValue);
  cpu.m_flags.updateNegativeAndZero(cpu.m_a.getContent());
}};

constexpr void executeSRE(CPU &cpu, u16 address) {}

const Instruction SRE = {.runAddress = [](CPU &cpu, u16 address) {
  u8 value = cpu.memory().read(address);
  cpu.m_flags.m_c = (value & 1) != 0;
  u8 newValue = value >> 1;
  cpu.memory().write(address, newValue);

  u8 finalA = cpu.m_a.getContent() ^ newValue;
  cpu.m_a.setContent(finalA);
  cpu.m_flags.updateNegativeAndZero(finalA);
}};

const Instruction RRA = {.runAddress = [](CPU &cpu, u16 address) {
  u8 value = cpu.memory().read(address);
  bool oldCarry = cpu.m_flags.m_c;

  cpu.m_flags.m_c = (value & 1) != 0;
  u8 newValue = (value >> 1) | (oldCarry ? 0x80 : 0x00);
  cpu.memory().write(address, newValue);

  const u8 oldValue = cpu.m_a.getContent();
  const u8 carryIn = cpu.m_flags.m_c ? 1 : 0;

  u16 sum = static_cast<u16>(oldValue) + static_cast<u16>(newValue) + carryIn;
  u8 finalA = static_cast<u8>(sum & 0xFF);

  cpu.m_flags.m_v = (~(oldValue ^ newValue) & (oldValue ^ finalA) & 0x80) != 0;
  cpu.m_flags.m_c = sum > 0xFF;
  cpu.m_a.setContent(finalA);
  cpu.m_flags.updateNegativeAndZero(finalA);
}};

const Instruction AAC = {.runValue = [](CPU &cpu, u8 value) {
  u8 result = cpu.m_a.getContent() & value;
  cpu.m_a.setContent(result);
  cpu.m_flags.updateNegativeAndZero(result);
  cpu.m_flags.m_c = (result & 0x80) != 0;
}};

const Instruction ASR = {.runValue = [](CPU &cpu, u8 value) {
  u8 result = cpu.m_a.getContent() & value;
  cpu.m_flags.m_c = result & 1;
  result >>= 1;
  cpu.m_a.setContent(result);
  cpu.m_flags.updateNegativeAndZero(result);
}};

const Instruction ARR = {.runValue = [](CPU &cpu, u8 value) {
  u8 andResult = cpu.m_a.getContent() & value;
  u8 oldCarry = cpu.m_flags.m_c ? 0x80 : 0x00;
  u8 finalA = (andResult >> 1) | oldCarry;

  cpu.m_a.setContent(finalA);
  cpu.m_flags.updateNegativeAndZero(finalA);

  cpu.m_flags.m_c = (finalA & 0x40) != 0;
  cpu.m_flags.m_v = (((finalA >> 6) ^ (finalA >> 5)) & 1);
}};

const Instruction ATX = {.runValue = [](CPU &cpu, u8 value) {
  cpu.m_a.setContent(value);
  cpu.m_x.setContent(value);
  cpu.m_flags.updateNegativeAndZero(value);
}};

const Instruction AXS = {.runValue = [](CPU &cpu, u8 value) {
  u8 sandx = cpu.m_a.getContent() & cpu.m_x.getContent();
  int diff = static_cast<int>(sandx) - static_cast<int>(value);
  u8 finalX = static_cast<u8>(diff & 0xFF);

  cpu.m_x.setContent(finalX);
  cpu.m_flags.m_z = (finalX == 0);
  cpu.m_flags.m_n = (finalX & 0x80) != 0;
  cpu.m_flags.m_c = (sandx >= value);
}};

const Instruction SYA = {.runAddress = [](CPU &cpu, u16 address) {
  u8 y = cpu.m_y.getContent();
  u8 highByte = static_cast<u8>((address >> 8) & 0xFF);
  u8 targetValue = y & (highByte + 1);

  u16 finalAddress = address;
  if (((address - cpu.m_x.getContent()) >> 8) != highByte) {
    finalAddress = (static_cast<u16>(targetValue) << 8) | (address & 0xFF);
  }

  cpu.memory().write(finalAddress, targetValue);
}};

const Instruction SXA = {.runAddress = [](CPU &cpu, u16 address) {
  u8 x = cpu.m_x.getContent();
  u8 highByte = static_cast<u8>((address >> 8) & 0xFF);
  u8 targetValue = x & (highByte + 1);

  u16 finalAddress = address;
  if (((address - cpu.m_y.getContent()) >> 8) != highByte) {
    finalAddress = (static_cast<u16>(targetValue) << 8) | (address & 0xFF);
  }

  cpu.memory().write(finalAddress, targetValue);
}};

} // namespace Instructions
