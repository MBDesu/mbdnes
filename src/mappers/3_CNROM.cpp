#include "mappers/3_CNROM.h"
#include "globals.h"

u8 CNROM::cpuRead(u16 address) {
  if (address >= 0x8000) {
    u16 mappedAddress = address - 0x8000;
    if (m_prgBankCount == 1) {
      mappedAddress %= 0x4000;
    }
    return m_cartridge->prg()[mappedAddress];
  }
  return 0;
}

void CNROM::cpuWrite(u16 address, u8 value) {
  if (address >= 0x8000) {
    m_chrBankSelect = value & 0x03;
  }
}

u8 CNROM::ppuRead(u16 address) {
  if (address >= 0x0000 && address <= 0x1fff) {
    size_t chrOffset = static_cast<size_t>(m_chrBankSelect) * 0x2000;
    return m_cartridge->chr()[chrOffset + address];
  }
  return 0;
}

void CNROM::ppuWrite(u16 address, u8 value) {
  if (address >= 0x0000 && address <= 0x1fff) {
    size_t chrOffset = static_cast<size_t>(m_chrBankSelect) * 0x2000;
    m_cartridge->chr()[chrOffset + address] = value;
  }
}
