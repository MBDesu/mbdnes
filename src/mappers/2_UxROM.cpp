#include "mappers/2_UxROM.h"

u8 UxROM::cpuRead(u16 address) {
  if (address < 0xc000) {
    return m_cartridge
        ->prg()[((address - 0x8000) & 0x3fff) | (m_prgBankSelect << 14)];
  }
  return m_cartridge
      ->prg()[m_cartridge->prg().size() - 0x4000 + (address & 0x3fff)];
}

void UxROM::cpuWrite(u16 address, u8 value) { m_prgBankSelect = value; }

u8 UxROM::ppuRead(u16 address) {
  if (m_cartridge->m_header.usesChrRam) {
    return m_ram[address];
  }
  return m_cartridge->chr()[address];
}

void UxROM::ppuWrite(u16 address, u8 value) {
  if (m_cartridge->m_header.usesChrRam) {
    m_ram[address] = value;
  }
}
