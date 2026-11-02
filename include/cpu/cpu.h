#pragma once
#ifndef MODULE_CPU_CPU_H
#define MODULE_CPU_CPU_H

#include "cpu_memory.h"
#include "globals.h"
#include "interrupts.h"

class Operation; // forward delcaration

template <typename T>
  requires std::same_as<T, u8> || std::same_as<T, u16>
class Register {
public:
  Register() : m_content{0} {};
  virtual ~Register() = default;
  virtual T getContent() const { return m_content; }
  virtual void setContent(const T content) { m_content = content; }
  virtual void increment() { setContent(getContent() + 1); }
  virtual void decrement() { setContent(getContent() - 1); }

protected:
  T m_content;
};

class FlagsRegister : public Register<u8> {

public:
  FlagsRegister()
      : Register<u8>(), m_c{false}, m_z{false}, m_i{false}, m_d{false},
        m_v{false}, m_n{false}, m_l{false}, m_b{false} {};
  bool m_c, m_z, m_i, m_d, m_v, m_n, m_l, m_b;

  u8 getValue() const {
    return (m_n << 7 | m_v << 6 | m_b << 4 | m_d << 3 | m_i << 2 | m_z << 1 |
            m_c);
  }

  void syncContent() {
    m_content = (m_n << 7) | (m_v << 6) | (1 << 5) | (m_b << 4) | (m_d << 3) |
                (m_i << 2) | (m_z << 1) | (m_c << 0);
  }

  void setValue(const u8 value) {
    m_n = (value >> 7) & 1;
    m_v = (value >> 6) & 1;
    m_d = (value >> 3) & 1;
    m_i = (value >> 2) & 1;
    m_z = (value >> 1) & 1;
    m_c = value & 1;
    m_content = value;
    syncContent();
  }

  void updateZero(const u8 value) { m_z = value == 0; }
  void updateNegative(const u8 value) { m_n = (value >> 7) & 1; }

  void updateNegativeAndZero(const u8 value) {
    updateZero(value);
    updateNegative(value);
    syncContent();
  }
};

class Stack {
public:
  Stack(CPUMemory *memory, Register<u8> *sp)
      : m_memory{memory}, m_sp{sp},
        m_currentAddress{static_cast<u16>(0x100 + m_sp->getContent())} {};
  virtual ~Stack() = default;

  void push(const u8 value) {
    m_memory->write(0x100 + m_sp->getContent(), value);
    m_sp->decrement();
  }

  void push16(const u16 value) {
    push((value & 0xff00) >> 8);
    push(value & 0xff);
  }

  u8 pop() {
    m_sp->increment();
    return m_memory->read(0x100 + m_sp->getContent());
  }

  u16 pop16() { return pop() | (pop() << 8); }

  void updateAddr() { m_currentAddress = 0x100 + m_sp->getContent(); }

private:
  CPUMemory *m_memory;
  Register<u8> *m_sp;
  u16 m_currentAddress;
};

struct CPULogFrame {
  u16 pc;
  u8 opcode;
  char instruction[32];
  u8 a;
  u8 x;
  u8 y;
  u8 sp;
  u8 status;
  u64 cycles;
};

class CPU {
public:
  Register<u16> m_pc;
  Register<u8> m_a;
  Register<u8> m_x;
  Register<u8> m_y;
  Register<u8> m_sp;
  FlagsRegister m_flags;
  size_t m_cycle;
  size_t m_extraCycles;
  Stack m_stack;

  explicit CPU(CPUMemory memory)
      : m_memory{memory}, m_pc{Register<u16>{}}, m_a{Register<u8>{}},
        m_x{Register<u8>{}}, m_y{Register<u8>{}}, m_sp{Register<u8>{}},
        m_stack{Stack{&m_memory, &m_sp}}, m_flags{FlagsRegister{}},
        m_extraCycles{0}, m_cycle{0} {};
  virtual ~CPU() = default;

  CPUMemory &memory() { return m_memory; }

  size_t addCycles(const Operation &operation);
  virtual u8 interrupt(Interrupt interrupt, bool withBFlag = false);
  virtual Operation fetchOperation();
  virtual u16 fetchInput(const Operation &operation);
  virtual u8 step();
  void dumpExecutionHistory() const;

  inline std::string_view resolveMnemonic(uint8_t opcode) {
    switch (opcode) {
    case 0x00:
      return "BRK";
    case 0xEA:
      return "NOP";
    case 0x40:
      return "RTI";
    case 0x60:
      return "RTS";

    case 0xA9:
    case 0xA5:
    case 0xB5:
    case 0xAD:
    case 0xBD:
    case 0xB9:
    case 0xA1:
    case 0x71:
      return "LDA";
    case 0xA2:
    case 0xA6:
    case 0xB6:
    case 0xAE:
    case 0xBE:
      return "LDX";
    case 0xA0:
    case 0xA4:
    case 0xB4:
    case 0xAC:
    case 0xBC:
      return "LDY";
    case 0x85:
    case 0x95:
    case 0x8D:
    case 0x9D:
    case 0x99:
    case 0x81:
    case 0x91:
      return "STA";
    case 0x86:
    case 0x96:
    case 0x8E:
      return "STX";
    case 0x84:
    case 0x94:
    case 0x8C:
      return "STY";

    case 0x69:
    case 0x65:
    case 0x75:
    case 0x6D:
    case 0x7D:
    case 0x79:
    case 0x61:
      return "ADC";
    case 0xE9:
    case 0xE5:
    case 0xF5:
    case 0xED:
    case 0xFD:
    case 0xF9:
    case 0xE1:
      return "SBC";
    case 0x29:
    case 0x25:
    case 0x35:
    case 0x2D:
    case 0x3D:
    case 0x39:
    case 0x21:
    case 0x31:
      return "AND";
    case 0x09:
    case 0x05:
    case 0x15:
    case 0x0D:
    case 0x1D:
    case 0x19:
    case 0x01:
    case 0x11:
      return "ORA";
    case 0x49:
    case 0x45:
    case 0x55:
    case 0x4D:
    case 0x5D:
    case 0x59:
    case 0x41:
    case 0x51:
      return "EOR";

    case 0xE6:
    case 0xF6:
    case 0xEE:
    case 0xFE:
      return "INC";
    case 0xE8:
      return "INX";
    case 0xC8:
      return "INY";
    case 0xC6:
    case 0xD6:
    case 0xCE:
    case 0xDE:
      return "DEC";
    case 0xCA:
      return "DEX";
    case 0x88:
      return "DEY";

    case 0x0A:
    case 0x06:
    case 0x16:
    case 0x0E:
    case 0x1E:
      return "ASL";
    case 0x4A:
    case 0x46:
    case 0x56:
    case 0x4E:
    case 0x5E:
      return "LSR";
    case 0x2A:
    case 0x26:
    case 0x36:
    case 0x2E:
    case 0x3E:
      return "ROL";
    case 0x6A:
    case 0x66:
    case 0x76:
    case 0x6E:
    case 0x7E:
      return "ROR";

    case 0x24:
    case 0x2C:
      return "BIT";
    case 0xC9:
    case 0xC5:
    case 0xD5:
    case 0xCD:
    case 0xDD:
    case 0xD9:
    case 0xC1:
    case 0xD1:
      return "CMP";
    case 0xE0:
    case 0xE4:
    case 0xEC:
      return "CPX";
    case 0xC0:
    case 0xC4:
    case 0xCC:
      return "CPY";

    case 0x4C:
    case 0x6C:
      return "JMP";
    case 0x20:
      return "JSR";
    case 0x10:
      return "BPL";
    case 0x30:
      return "BMI";
    case 0x50:
      return "BVC";
    case 0x70:
      return "BVS";
    case 0x90:
      return "BCC";
    case 0xB0:
      return "BCS";
    case 0xD0:
      return "BNE";
    case 0xF0:
      return "BEQ";

    case 0x48:
      return "PHA";
    case 0x08:
      return "PHP";
    case 0x68:
      return "PLA";
    case 0x28:
      return "PLP";

    case 0xAA:
      return "TAX";
    case 0xA8:
      return "TAY";
    case 0xBA:
      return "TSX";
    case 0x8A:
      return "TXA";
    case 0x9A:
      return "TXS";
    case 0x98:
      return "TYA";

    case 0x18:
      return "CLC";
    case 0x38:
      return "SEC";
    case 0x58:
      return "CLI";
    case 0x78:
      return "SEI";
    case 0xD8:
      return "CLD";
    case 0xF8:
      return "SED";
    case 0xB8:
      return "CLV";

    default:
      return "???";
    }
  }

private:
  CPUMemory m_memory;
  mutable std::vector<CPULogFrame> m_executionHistory;
  const size_t MAX_LOG_FRAMES = 100;
};

#endif // MODULE_CPU_CPU_H
