#include "cpu/addressing_modes.h"

const ImplicitMode _impl;
const ImmediateMode _imm;
const AbsoluteMode _abs;
const ZeroPageMode _zp;
const RelativeMode _rel;
const IndirectMode _ind;
const IndexedZeroPageXMode _zpx;
const IndexedZeroPageYMode _zpy;
const IndexedAbsoluteXMode _absx;
const IndexedAbsoluteYMode _absy;
const IndexedIndirectMode _indx;
const IndirectIndexedMode _indy;

u8 AddressingMode::readMemory(CPU &cpu, u16 address) const {
  return cpu.memory().read(address);
}

// RELATIVE
u16 RelativeMode::getAddress(CPU &cpu, u16 offset,
                             bool hasPageCrossPenalty) const {
  u16 pcValue = cpu.m_pc.getContent();
  u16 address = static_cast<u16>(pcValue + static_cast<s8>(offset & 0xff));

  // if (hasPageCrossPenalty &&
  //     ((address >> 8) & 0xff) != ((pcValue >> 8) & 0xff)) {
  //   cpu.m_extraCycles++;
  // }
  return address;
}

// INDIRECT
u16 IndirectMode::getAddress(CPU &cpu, u16 absoluteAddress,
                             bool hasPageCrossPenalty) const {
  u16 target = ((absoluteAddress & 0xFF) == 0xFF)
                   ? static_cast<u16>((absoluteAddress & 0xFF00))
                   : static_cast<u16>(absoluteAddress + 1);

  u16 low = cpu.memory().read(absoluteAddress);
  u16 high = cpu.memory().read(target);
  return (high << 8) | low;
}

// INDEXED_ZERO_PAGE_X
u16 IndexedZeroPageXMode::getAddress(CPU &cpu, u16 zeroPageAddress,
                                     bool hasPageCrossPenalty) const {
  return (zeroPageAddress + cpu.m_x.getContent()) & 0xFF;
}

// INDEXED_ZERO_PAGE_Y
u16 IndexedZeroPageYMode::getAddress(CPU &cpu, u16 zeroPageAddress,
                                     bool hasPageCrossPenalty) const {
  return (zeroPageAddress + cpu.m_y.getContent()) & 0xFF;
}

// INDEXED_ABSOLUTE_X
u16 IndexedAbsoluteXMode::getAddress(CPU &cpu, u16 absoluteAddress,
                                     bool hasPageCrossPenalty) const {
  u16 address = absoluteAddress + cpu.m_x.getContent();
  if (((address >> 8) & 0xff) != ((absoluteAddress >> 8) & 0xff)) {
    if (hasPageCrossPenalty) {
      cpu.m_extraCycles++;
    }
    u16 dummy = (absoluteAddress & 0xff00) | (address & 0x00ff);
    cpu.memory().read(dummy);
  }
  return address;
}

// INDEXED_ABSOLUTE_Y
u16 IndexedAbsoluteYMode::getAddress(CPU &cpu, u16 absoluteAddress,
                                     bool hasPageCrossPenalty) const {
  u16 address = absoluteAddress + cpu.m_y.getContent();
  if (((address >> 8) & 0xFF) != ((absoluteAddress >> 8) & 0xFF)) {
    if (hasPageCrossPenalty) {
      cpu.m_extraCycles++;
    }
    u16 dummy = (absoluteAddress & 0xff00) | (address & 0x00ff);
    cpu.memory().read(dummy);
  }
  return address;
}

// INDEXED_INDIRECT
u16 IndexedIndirectMode::getAddress(CPU &cpu, u16 zeroPageAddress,
                                    bool hasPageCrossPenalty) const {
  u8 start = (cpu.m_x.getContent() + zeroPageAddress) & 0xFF;
  u8 end = (start + 1) & 0xFF;

  u16 low = cpu.memory().read(start);
  u16 high = cpu.memory().read(end);
  return (high << 8) | low;
}

// INDIRECT_INDEXED
u16 IndirectIndexedMode::getAddress(CPU &cpu, u16 zeroPageAddress,
                                    bool hasPageCrossPenalty) const {
  u8 start = zeroPageAddress & 0xff;
  u8 end = (start + 1) & 0xff;

  u16 low = cpu.memory().read(start);
  u16 high = cpu.memory().read(end);
  u16 baseAddress = (high << 8) | low;
  u16 address = baseAddress + cpu.m_y.getContent();

  if (((baseAddress >> 8) & 0xFF) != ((address >> 8) & 0xFF)) {
    if (hasPageCrossPenalty) {
      cpu.m_extraCycles++;
    }
    u16 dummy = (baseAddress & 0xff00) | (address & 0x00ff);
    cpu.memory().read(dummy);
  }
  return address;
}
