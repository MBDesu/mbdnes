#include "mappers/0_NROM.h"

u8 NROM::cpuRead(const u16 address) {
  if (address >= 0x4020 && address < 0x8000) {
    // unused
  } else if (address >= 0x8000 && address < 0xc000) {
    // CPU $8000-$bfff : first 16 KB of PRG-ROM
    return $getPrgPage(0)[address - 0x8000];
  } else if (address >= 0xc000) {
    // CPU $c000 - $ffff: last 16 KB of PRG-ROM (or mirror of $8000-$bfff)
    return $getPrgPage(1)[address - 0xc000];
  }
  return 0;
}

void NROM::ppuWrite(const u16 address, const u8 value) {
  if (!m_cartridge->m_header.usesChrRam)
    return;
  $getChrPage(0)[address] = value;
}
