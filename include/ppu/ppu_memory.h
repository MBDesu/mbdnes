#ifndef MODULE_PPU_PPU_MEMORY_H
#define MODULE_PPU_PPU_MEMORY_H

#pragma once

#include "cartridge.h"
#include "globals.h"

#include <array>
#include <memory>
#include <string_view>

class Mapper; // forward declaration

enum MirroringTypes {
  HORIZONTAL,
  VERTICAL,
  ONE_SCREEN_LOWER_BANK,
  ONE_SCREEN_UPPER_BANK,
  FOUR_SCREEN,
  NUM_MIRRORING_TYPES
};

struct MirroringType {
  std::string_view id;
  u16 $2000;
  u16 $2400;
  u16 $2800;
  u16 $2c00;
};

const std::array<MirroringType, MirroringTypes::NUM_MIRRORING_TYPES>
    MIRRORING_TYPES = {{{.id = "HORIZONTAL",
                         .$2000 = 0x000,
                         .$2400 = 0x000,
                         .$2800 = 0x400,
                         .$2c00 = 0x400},
                        {.id = "VERTICAL",
                         .$2000 = 0x000,
                         .$2400 = 0x400,
                         .$2800 = 0x000,
                         .$2c00 = 0x400},
                        {.id = "ONE_SCREEN_LOWER_BANK",
                         .$2000 = 0x000,
                         .$2400 = 0x000,
                         .$2800 = 0x000,
                         .$2c00 = 0x000},
                        {.id = "ONE_SCREEN_UPPER_BANK",
                         .$2000 = 0x400,
                         .$2400 = 0x400,
                         .$2800 = 0x400,
                         .$2c00 = 0x400},
                        {.id = "FOUR_SCREEN",
                         .$2000 = 0x000,
                         .$2400 = 0x400,
                         .$2800 = 0x800,
                         .$2c00 = 0xc00}}};

class PPUMemory {
public:
  std::array<u8, 4096> m_vRam;
  std::array<u8, 32> m_paletteRam;
  std::array<u8, 256> m_oamRam;
  std::string_view m_mirroringId;
  std::shared_ptr<Mapper> m_mapper;
  std::shared_ptr<Cartridge> m_cartridge;

  PPUMemory() {
    m_vRam.fill(0);
    m_paletteRam.fill(0);
    m_oamRam.fill(0);
  }

  PPUMemory(std::shared_ptr<Mapper> mapper,
            std::shared_ptr<Cartridge> cartridge) {
    m_vRam.fill(0);
    m_paletteRam.fill(0);
    m_oamRam.fill(0);
  };
  ~PPUMemory() = default;

  virtual void onLoad(std::shared_ptr<Cartridge> cartridge,
                      std::shared_ptr<Mapper> mapper);
  virtual u8 read(u16 address);
  virtual void write(u16 address, u8 value);
  virtual void changeNameTableMirroringTo(std::string_view mirroringId);

private:
  MirroringType m_mirroring;
};

#endif // MODULE_PPU_PPU_MEMORY_H
