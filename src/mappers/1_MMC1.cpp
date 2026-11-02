#include "mappers/1_MMC1.h"
#include "ppu/ppu.h"

MMC1::MMC1(std::shared_ptr<CPU> cpu, std::shared_ptr<PPU> ppu,
           std::shared_ptr<Cartridge> cartridge)
    : Mapper(cpu, ppu, cartridge) {
  m_prgPageCount = m_cartridge->m_header.prgRomPages;
  m_chrPageCount4KB = m_cartridge->m_header.chrRomPages * 2;

  if (m_cartridge->m_header.mirroringId == "HORIZONTAL") {
    m_mirroringMode = 3;
    m_control = 0x03;
  } else if (m_cartridge->m_header.mirroringId == "VERTICAL") {
    m_mirroringMode = 2;
    m_control = 0x02;
  } else {
    m_mirroringMode = 0;
    m_control = 0x00;
  }

  if (m_ppu != nullptr) {
    if (m_mirroringMode == 2)
      m_ppu->memory().changeNameTableMirroringTo("VERTICAL");
    if (m_mirroringMode == 3)
      m_ppu->memory().changeNameTableMirroringTo("HORIZONTAL");
  }

  m_prgRomMode = 3;
  m_chrRomMode = 0;
  onLoad();
}

void MMC1::onLoad() { updateBankMappings(); }

void MMC1::resetShiftRegister() { m_shiftRegister = 0x10; }

void MMC1::cpuWrite(u16 address, u8 value) {
  if (address < 0x8000)
    return;

  u64 currentCycle = m_cpu->m_cycle;
  if (currentCycle == m_lastWriteCycle ||
      currentCycle == m_lastWriteCycle + 1) {
    return;
  }
  m_lastWriteCycle = currentCycle;

  if ((value & 0x80) != 0) {
    m_control |= 0x0c;
    resetShiftRegister();
    updateBankMappings();
    return;
  }

  bool shiftComplete = (m_shiftRegister & 1) != 0;
  m_shiftRegister >>= 1;
  m_shiftRegister |= ((value & 1) << 4);

  if (shiftComplete) {
    writeRegister(address, m_shiftRegister);
    resetShiftRegister();
  }
}

void MMC1::writeRegister(u16 address, u8 value) {
  if (address >= 0x8000 && address <= 0x9fff) {
    m_control = value;
    m_mirroringMode = m_control & 0x03;
    m_prgRomMode = (m_control >> 2) & 0x03;
    m_chrRomMode = (m_control >> 4) & 0x01;
  } else if (address >= 0xa000 && address <= 0xbfff) {
    m_chrBank0 = value;
  } else if (address >= 0xc000 && address <= 0xdfff) {
    m_chrBank1 = value;
  } else if (address >= 0xe000 && address <= 0xffff) {
    m_prgBank = value & 0x0f;
  }
  updateBankMappings();
}
void MMC1::updateBankMappings() {
  if (!m_ppu)
    return;

  switch (m_mirroringMode) {
  case 0:
    m_ppu->memory().changeNameTableMirroringTo("ONE_SCREEN_LOWER_BANK");
    break;
  case 1:
    m_ppu->memory().changeNameTableMirroringTo("ONE_SCREEN_UPPER_BANK");
    break;
  case 2:
    m_ppu->memory().changeNameTableMirroringTo("VERTICAL");
    break;
  case 3:
    m_ppu->memory().changeNameTableMirroringTo("HORIZONTAL");
    break;
  }
}

u8 MMC1::cpuRead(u16 address) {
  if (address >= 0x8000) {
    u16 offset = address & 0x3fff;
    size_t firstBank = 0;
    size_t secondBank = 0;

    switch (m_prgRomMode) {
    case 0:
    case 1:
      return m_cartridge
          ->prg()[(static_cast<size_t>(m_prgBank & 0x0e) * 16384) +
                  (address & 0x7fff)];

    case 2:
      firstBank = 0;
      secondBank = m_prgBank;
      break;

    case 3:
      firstBank = m_prgBank;
      secondBank = m_prgPageCount - 1;
      break;
    }

    if (address < 0xC000) {
      return m_cartridge->prg()[(firstBank * 16384) + offset];
    } else {
      return m_cartridge->prg()[(secondBank * 16384) + offset];
    }
  }
  return 0;
}

u8 MMC1::ppuRead(u16 address) {
  if (address <= 0x1FFF) {
    if (m_cartridge->m_header.usesChrRam) {
      return m_cartridge->chr()[address];
    }

    if (m_chrRomMode == 0) {
      size_t bank = (m_chrBank0 & 0x1e) * 4096;
      return m_cartridge->chr()[bank + address];
    } else {
      u16 offset = address & 0x0fff;
      if (address < 0x1000) {
        return m_cartridge
            ->chr()[(static_cast<size_t>(m_chrBank0) * 4096) + offset];
      } else {
        return m_cartridge
            ->chr()[(static_cast<size_t>(m_chrBank1) * 4096) + offset];
      }
    }
  }
  return 0;
}

void MMC1::ppuWrite(u16 address, u8 value) {
  if (address <= 0x1fff && m_cartridge->m_header.usesChrRam) {
    m_cartridge->chr()[address] = value;
  }
}

void MMC1::tick() {}
