#pragma once
#ifndef MODULE_MAPPERS_3_CNROM_H
#define MODULE_MAPPERS_3_CNROM_H

#include "mapper.h"

class CNROM : public Mapper {
public:
  using Mapper::Mapper;
  CNROM(std::shared_ptr<CPU> cpu, std::shared_ptr<PPU> ppu,
        std::shared_ptr<Cartridge> cart)
      : Mapper(cpu, ppu, cart) {
    // CNROM uses standard 16KB PRG pages [1]
    m_prgBankCount = m_cartridge->m_header.prgRomPages;
  }

  u8 cpuRead(u16 address) override;
  void cpuWrite(u16 address, u8 value) override;
  u8 ppuRead(u16 address) override;
  void ppuWrite(u16 address, u8 value) override;
  void tick() override {};

private:
  u8 m_chrBankSelect = 0;
  size_t m_prgBankCount = 0;
};

#endif // MODULE_MAPPERS_3_CNROM_H
