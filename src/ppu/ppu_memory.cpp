#include "ppu/ppu_memory.h"
#include "cartridge.h"
#include "mapper.h"

void PPUMemory::onLoad(std::shared_ptr<Cartridge> cartridge,
                       std::shared_ptr<Mapper> mapper) {
  m_cartridge = cartridge;
  m_mapper = mapper;
  changeNameTableMirroringTo(cartridge->m_header.mirroringId);
}

void PPUMemory::changeNameTableMirroringTo(std::string_view mirroringId) {
  if (m_cartridge->m_header.mirroringId == "FOUR_SCREEN") {
    m_mirroring = MIRRORING_TYPES[MirroringTypes::FOUR_SCREEN];
  }
  for (size_t i = 0; i < MirroringTypes::NUM_MIRRORING_TYPES; i++) {
    if (mirroringId == MIRRORING_TYPES[i].id) {
      m_mirroring = MIRRORING_TYPES[i];
      break;
    }
  }
  m_mirroringId = m_mirroring.id;
}

u8 PPUMemory::read(u16 address) {
  // Pattern tables 0 and 1 (mapper)
  if (address >= 0x0000 && address <= 0x1fff) {
    return m_mapper->ppuRead(address);
  }

  // Name tables 0 to 3 (vRam + mirror)
  if (address >= 0x2000 && address <= 0x2fff) {
    if (address >= 0x2000 && address < 0x2400) {
      return m_vRam[m_mirroring.$2000 + (address - 0x2000)];
    }
    if (address >= 0x2400 && address < 0x2800) {
      return m_vRam[m_mirroring.$2400 + (address - 0x2400)];
    }
    if (address >= 0x2800 && address < 0x2c00) {
      return m_vRam[m_mirroring.$2800 + (address - 0x2800)];
    }
    if (address >= 0x2c00 && address < 0x3000) {
      return m_vRam[m_mirroring.$2c00 + (address - 0x2c00)];
    }
    return m_vRam[address - 0x2000];
  }

  // Mirrors of $2000-$2EFF
  if (address >= 0x3000 && address <= 0x3eff)
    return read(0x2000 + ((address - 0x3000) % 0x1000));

  // Palette RAM
  if (address >= 0x3f00 && address <= 0x3fff) {
    u16 paletteAddr = (address - 0x3f00) % 0x0020;
    if (paletteAddr == 0x0010 || paletteAddr == 0x0014 ||
        paletteAddr == 0x0018 || paletteAddr == 0x001c) {
      paletteAddr -= 0x0010;
    }
    return m_paletteRam[paletteAddr];
  }

  return 0;
}

void PPUMemory::write(u16 address, u8 value) {
  // 🕊️ Pattern tables 0 and 1 (mapper)
  if (address >= 0x0000 && address <= 0x1fff) {
    m_mapper->ppuWrite(address, value);
    return;
  }

  // Name tables 0 to 3 (vRam + mirror)
  if (address >= 0x2000 && address <= 0x2fff) {
    if (address >= 0x2000 && address < 0x2400) {
      m_vRam[m_mirroring.$2000 + (address - 0x2000)] = value;
      return;
    }
    if (address >= 0x2400 && address < 0x2800) {
      m_vRam[m_mirroring.$2400 + (address - 0x2400)] = value;
      return;
    }
    if (address >= 0x2800 && address < 0x2c00) {
      m_vRam[m_mirroring.$2800 + (address - 0x2800)] = value;
      return;
    }
    if (address >= 0x2c00 && address < 0x3000) {
      m_vRam[m_mirroring.$2c00 + (address - 0x2c00)] = value;
      return;
    }
    m_vRam[address - 0x2000] = value;
    return;
  }

  // Mirrors of $2000-$2EFF
  if (address >= 0x3000 && address <= 0x3eff) {
    write(0x2000 + ((address - 0x3000) % 0x1000), value);
    return;
  }

  // Palette RAM + Mirrors ($3F00–$3FFF)
  if (address >= 0x3f00 && address <= 0x3fff) {
    u16 paletteAddr = (address - 0x3f00) % 0x0020;
    if (paletteAddr == 0x0010 || paletteAddr == 0x0014 ||
        paletteAddr == 0x0018 || paletteAddr == 0x001c) {
      paletteAddr -= 0x0010;
    }
    m_paletteRam[paletteAddr] = value;
    return;
  }
}
