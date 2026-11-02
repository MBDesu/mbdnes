#pragma once
#ifndef MODULE_MAPPERS_1_MMC1_H
#define MODULE_MAPPERS_1_MMC1_H

#include "mapper.h"

class MMC1 : public Mapper {
private:
  u64 m_lastWriteCycle = 0;
  u8 m_shiftRegister = 0x10;

  u8 m_control = 0x0c;
  u8 m_chrBank0 = 0;
  u8 m_chrBank1 = 0;
  u8 m_prgBank = 0;

  u8 m_mirroringMode = 0;
  u8 m_prgRomMode = 3;
  u8 m_chrRomMode = 0;

  size_t m_prgPageCount = 0;
  size_t m_chrPageCount4KB = 0;

  void resetShiftRegister();
  void writeRegister(u16 address, u8 value);
  void updateBankMappings();

public:
  MMC1(std::shared_ptr<CPU> cpu, std::shared_ptr<PPU> ppu,
       std::shared_ptr<Cartridge> cartridge);
  virtual ~MMC1() = default;

  void onLoad() override;
  u8 cpuRead(u16 address) override;
  void cpuWrite(u16 address, u8 value) override;
  u8 ppuRead(u16 address) override;
  void ppuWrite(u16 address, u8 value) override;
  void tick() override;
};

#endif // MODULE_MAPPERS_1_MMC1_H
