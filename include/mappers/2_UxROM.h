#pragma once
#ifndef MODULE_MAPPERS_2_UXROM_H
#define MODULE_MAPPERS_2_UXROM_H

#include "mapper.h"

class UxROM : public Mapper {
public:
  UxROM(std::shared_ptr<CPU> cpu, std::shared_ptr<PPU> ppu,
        std::shared_ptr<Cartridge> cartridge)
      : Mapper(cpu, ppu, cartridge) {
    if (cartridge->m_header.usesChrRam) {
      m_ram.resize(0x2000, 0x0);
    }
  }
  u8 cpuRead(u16 address) override;
  void cpuWrite(u16 address, u8 value) override;
  u8 ppuRead(u16 address) override;
  void ppuWrite(u16 address, u8 value) override;
  void tick() override {};

private:
  bool m_usesChrRam = false;
  u8 m_prgBankSelect = 0;
  size_t m_prgBankCount = 0;
  std::vector<u8> m_ram;
};

#endif // MODULE_MAPPERS_2_UXROM_H
