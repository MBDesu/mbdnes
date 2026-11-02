#pragma once
#ifndef MODULE_MAPPERS_4_MMC3_H
#define MODULE_MAPPERS_4_MMC3_H

#include "mapper.h"
#include <array>
#include <vector>

class MMC3 : public Mapper {
public:
  MMC3(std::shared_ptr<CPU> cpu, std::shared_ptr<PPU> ppu,
       std::shared_ptr<Cartridge> cartridge);
  virtual ~MMC3() = default;

  u8 cpuRead(u16 address) override;
  void cpuWrite(u16 address, u8 value) override;
  u8 ppuRead(u16 address) override;
  void ppuWrite(u16 address, u8 value) override;
  void tick() override;

private:
  void updatePrgBanks();
  void updateChrBanks();
  void checkA12Toggle(u16 address);

  u8 m_bankSelect = 0;
  std::array<u8, 8> m_bankRegisters = {0};
  u8 m_prgRamProtect = 0;

  u8 m_irqLatch = 0;
  u8 m_irqCounter = 0;
  bool m_irqEnabled = false;
  bool m_irqReload = false;
  bool m_lastA12State = false;
  int m_a12LowTimer = 0;

  size_t m_prgPageCount8KB = 0;
  std::vector<u8> m_prgRAM;

  std::array<size_t, 4> m_prgOffsets = {0};
  std::array<size_t, 8> m_chrOffsets = {0};
};

#endif // MODULE_MAPPERS_4_MMC3_H
