#pragma once
#ifndef MODULE_CPU_ADDRESSING_MODES_H
#define MODULE_CPU_ADDRESSING_MODES_H

#include "cpu.h"

// sure hope no one modifies these
// (indexing via enum is unsafe, but who cares)
enum class AddressingModeId : size_t {
  IMPLICIT,
  IMMEDIATE,
  ABSOLUTE,
  ZERO_PAGE,
  RELATIVE,
  INDIRECT,
  INDEXED_ZERO_PAGE_X,
  INDEXED_ZERO_PAGE_Y,
  INDEXED_ABSOLUTE_X,
  INDEXED_ABSOLUTE_Y,
  INDEXED_INDIRECT,
  INDIRECT_INDEXED,
  NUM_ADDR_MODES,
};

class AddressingMode {
public:
  const AddressingModeId m_id;
  const int m_inputSize;

  AddressingMode(AddressingModeId modeId, int size)
      : m_id{modeId}, m_inputSize{size} {}
  virtual ~AddressingMode() = default;

  virtual u16 getAddress(CPU &cpu, u16 argument,
                         bool hasPageCrossPenalty) const {
    throw std::runtime_error("Unsupported getAddress call.");
  }

  virtual u8 getValue(CPU &cpu, u16 argument, bool hasPageCrossPenalty) const {
    return readMemory(cpu, getAddress(cpu, argument, hasPageCrossPenalty));
  }

protected:
  u8 readMemory(CPU &cpu, u16 address) const;
};

class ImplicitMode final : public AddressingMode {
public:
  ImplicitMode() : AddressingMode(AddressingModeId::IMPLICIT, 0) {}

  u16 getAddress(CPU &cpu, u16 argument,
                 bool hasPageCrossPenalty) const override {
    return 0;
  }

  u8 getValue(CPU &cpu, u16 argument, bool hasPageCrossPenalty) const override {
    throw std::runtime_error("Unsupported.");
  }
};

class ImmediateMode final : public AddressingMode {
public:
  ImmediateMode() : AddressingMode(AddressingModeId::IMMEDIATE, 1) {}
  u16 getAddress(CPU &cpu, u16 argument,
                 bool hasPageCrossPenalty) const override {
    throw std::runtime_error("Unsupported.");
  }
  u8 getValue(CPU &cpu, u16 value, bool hasPageCrossPenalty) const override {
    return static_cast<u8>(value);
  }
};

class AbsoluteMode final : public AddressingMode {
public:
  AbsoluteMode() : AddressingMode(AddressingModeId::ABSOLUTE, 2) {}
  u16 getAddress(CPU &cpu, u16 address,
                 bool hasPageCrossPenalty) const override {
    return address;
  }
};

class ZeroPageMode final : public AddressingMode {
public:
  ZeroPageMode() : AddressingMode(AddressingModeId::ZERO_PAGE, 1) {}
  u16 getAddress(CPU &cpu, u16 zeroPageAddress,
                 bool hasPageCrossPenalty) const override {
    return zeroPageAddress & 0xFF;
  }
};

class RelativeMode final : public AddressingMode {
public:
  RelativeMode() : AddressingMode(AddressingModeId::RELATIVE, 1) {}
  u16 getAddress(CPU &cpu, u16 offset, bool hasPageCrossPenalty) const override;
  u8 getValue(CPU &cpu, u16 argument, bool hasPageCrossPenalty) const override {
    throw std::runtime_error("Unsupported.");
  }
};

class IndirectMode final : public AddressingMode {
public:
  IndirectMode() : AddressingMode(AddressingModeId::INDIRECT, 2) {}
  u16 getAddress(CPU &cpu, u16 absoluteAddress,
                 bool hasPageCrossPenalty) const override;
  u8 getValue(CPU &cpu, u16 argument, bool hasPageCrossPenalty) const override {
    throw std::runtime_error("Unsupported.");
  }
};

class IndexedZeroPageXMode final : public AddressingMode {
public:
  IndexedZeroPageXMode()
      : AddressingMode(AddressingModeId::INDEXED_ZERO_PAGE_X, 1) {}
  u16 getAddress(CPU &cpu, u16 zeroPageAddress,
                 bool hasPageCrossPenalty) const override;
};

class IndexedZeroPageYMode final : public AddressingMode {
public:
  IndexedZeroPageYMode()
      : AddressingMode(AddressingModeId::INDEXED_ZERO_PAGE_Y, 1) {}
  u16 getAddress(CPU &cpu, u16 zeroPageAddress,
                 bool hasPageCrossPenalty) const override;
};

class IndexedAbsoluteXMode final : public AddressingMode {
public:
  IndexedAbsoluteXMode()
      : AddressingMode(AddressingModeId::INDEXED_ABSOLUTE_X, 2) {}
  u16 getAddress(CPU &cpu, u16 absoluteAddress,
                 bool hasPageCrossPenalty) const override;
};

class IndexedAbsoluteYMode final : public AddressingMode {
public:
  IndexedAbsoluteYMode()
      : AddressingMode(AddressingModeId::INDEXED_ABSOLUTE_Y, 2) {}
  u16 getAddress(CPU &cpu, u16 absoluteAddress,
                 bool hasPageCrossPenalty) const override;
};

class IndexedIndirectMode final : public AddressingMode {
public:
  IndexedIndirectMode()
      : AddressingMode(AddressingModeId::INDEXED_INDIRECT, 1) {}
  u16 getAddress(CPU &cpu, u16 zeroPageAddress,
                 bool hasPageCrossPenalty) const override;
};

class IndirectIndexedMode final : public AddressingMode {
public:
  IndirectIndexedMode()
      : AddressingMode(AddressingModeId::INDIRECT_INDEXED, 1) {}
  u16 getAddress(CPU &cpu, u16 zeroPageAddress,
                 bool hasPageCrossPenalty) const override;
};

extern const ImplicitMode _impl;
extern const ImmediateMode _imm;
extern const AbsoluteMode _abs;
extern const ZeroPageMode _zp;
extern const RelativeMode _rel;
extern const IndirectMode _ind;
extern const IndexedZeroPageXMode _zpx;
extern const IndexedZeroPageYMode _zpy;
extern const IndexedAbsoluteXMode _absx;
extern const IndexedAbsoluteYMode _absy;
extern const IndexedIndirectMode _indx;
extern const IndirectIndexedMode _indy;

const std::array<const AddressingMode *,
                 static_cast<size_t>(AddressingModeId::NUM_ADDR_MODES)>
    ADDRESSING_MODES = {&_impl, &_imm, &_abs,  &_zp,   &_rel,  &_ind,
                        &_zpx,  &_zpy, &_absx, &_absy, &_indx, &_indy};

#endif // MODULE_CPU_ADDRESSING_MODES_H
