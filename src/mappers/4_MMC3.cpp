#include "mappers/4_MMC3.h"
#include "ppu/ppu.h"

MMC3::MMC3(std::shared_ptr<CPU> cpu, std::shared_ptr<PPU> ppu,
           std::shared_ptr<Cartridge> cartridge)
    : Mapper(cpu, ppu, cartridge) {
  m_prgPageCount8KB = m_cartridge->prg().size() / 8192;
  m_prgRAM.resize(0x2000, 0x00);

  updatePrgBanks();
  updateChrBanks();
}

void MMC3::updatePrgBanks() {
  bool prgMode = (m_bankSelect & 0x40) != 0;

  if (!prgMode) {
    m_prgOffsets[0] = static_cast<size_t>(m_bankRegisters[6]) * 8192;
    m_prgOffsets[1] = static_cast<size_t>(m_bankRegisters[7]) * 8192;
    m_prgOffsets[2] = (m_prgPageCount8KB - 2) * 8192;
  } else {
    m_prgOffsets[0] = (m_prgPageCount8KB - 2) * 8192;
    m_prgOffsets[1] = static_cast<size_t>(m_bankRegisters[7]) * 8192;
    m_prgOffsets[2] = static_cast<size_t>(m_bankRegisters[6]) * 8192;
  }
  m_prgOffsets[3] = (m_prgPageCount8KB - 1) * 8192;
}

void MMC3::updateChrBanks() {
  bool chrMode = (m_bankSelect & 0x80) != 0;

  if (!chrMode) {
    m_chrOffsets[0] = static_cast<size_t>(m_bankRegisters[0] & 0xfe) * 1024;
    m_chrOffsets[1] = static_cast<size_t>(m_bankRegisters[0] | 0x01) * 1024;
    m_chrOffsets[2] = static_cast<size_t>(m_bankRegisters[1] & 0xfe) * 1024;
    m_chrOffsets[3] = static_cast<size_t>(m_bankRegisters[1] | 0x01) * 1024;
    m_chrOffsets[4] = static_cast<size_t>(m_bankRegisters[2]) * 1024;
    m_chrOffsets[5] = static_cast<size_t>(m_bankRegisters[3]) * 1024;
    m_chrOffsets[6] = static_cast<size_t>(m_bankRegisters[4]) * 1024;
    m_chrOffsets[7] = static_cast<size_t>(m_bankRegisters[5]) * 1024;
  } else {
    m_chrOffsets[4] = static_cast<size_t>(m_bankRegisters[0] & 0xfe) * 1024;
    m_chrOffsets[5] = static_cast<size_t>(m_bankRegisters[0] | 0x01) * 1024;
    m_chrOffsets[6] = static_cast<size_t>(m_bankRegisters[1] & 0xfe) * 1024;
    m_chrOffsets[7] = static_cast<size_t>(m_bankRegisters[1] | 0x01) * 1024;
    m_chrOffsets[0] = static_cast<size_t>(m_bankRegisters[2]) * 1024;
    m_chrOffsets[1] = static_cast<size_t>(m_bankRegisters[3]) * 1024;
    m_chrOffsets[2] = static_cast<size_t>(m_bankRegisters[4]) * 1024;
    m_chrOffsets[3] = static_cast<size_t>(m_bankRegisters[5]) * 1024;
  }
}

void MMC3::checkA12Toggle(u16 address) {
  bool currentA12 = (address & 0x1000) != 0;

  if (currentA12 && !m_lastA12State && m_a12LowTimer > 10) {
    if (m_irqCounter == 0 || m_irqReload) {
      m_irqCounter = m_irqLatch;
      m_irqReload = false;
    } else {
      m_irqCounter--;
    }

    if (m_irqCounter == 0 && m_irqEnabled) {
      m_cpu->interrupt(INTERRUPTS[Interrupts::IRQ], false);
    }
  }

  if (!currentA12) {
    m_a12LowTimer++;
  } else {
    m_a12LowTimer = 0;
  }
  m_lastA12State = currentA12;
}

u8 MMC3::cpuRead(u16 address) {
  if (address >= 0x6000 && address <= 0x7fff) {
    return m_prgRAM[address & 0x1fff];
  }
  if (address >= 0x8000) {
    u16 offset = address & 0x1fff;
    size_t slot = (address - 0x8000) / 0x2000;
    return m_cartridge
        ->prg()[(m_prgOffsets[slot] + offset) % m_cartridge->prg().size()];
  }
  return 0;
}

void MMC3::cpuWrite(u16 address, u8 value) {
  if (address >= 0x6000 && address <= 0x7fff) {
    m_prgRAM[address & 0x1fff] = value;
    return;
  }
  if (address >= 0x8000 && address <= 0x9fff) {
    if ((address & 1) == 0) {
      m_bankSelect = value;
      updatePrgBanks();
      updateChrBanks();
    } else {
      u8 cmd = m_bankSelect & 0x07;
      m_bankRegisters[cmd] = value;
      updatePrgBanks();
      updateChrBanks();
    }
  } else if (address >= 0xa000 && address <= 0xbfff) {
    if ((address & 1) == 0) {
      std::string_view nextMode = (value & 1) ? "HORIZONTAL" : "VERTICAL";
      m_ppu->memory().changeNameTableMirroringTo(nextMode);
    } else {
      m_prgRamProtect = value;
    }
  } else if (address >= 0xc000 && address <= 0xdfff) {
    if ((address & 1) == 0) {
      m_irqLatch = value;
    } else {
      m_irqReload = true;
    }
  } else if (address >= 0xe000 && address <= 0xffff) {
    if ((address & 1) == 0) {
      m_irqEnabled = false;
    } else {
      m_irqEnabled = true;
    }
  }
}

u8 MMC3::ppuRead(u16 address) {
  if (address <= 0x1fff) {
    checkA12Toggle(address);
    u16 offset = address & 0x03ff;
    size_t slot = address / 0x0400;
    if (m_cartridge->chr().size() == 0)
      return 0;
    return m_cartridge
        ->chr()[(m_chrOffsets[slot] + offset) % m_cartridge->chr().size()];
  }
  return 0;
}

void MMC3::ppuWrite(u16 address, u8 value) {
  if (address <= 0x1fff) {
    checkA12Toggle(address);
    if (m_cartridge->m_header.usesChrRam) {
      u16 offset = address & 0x03ff;
      size_t slot = address / 0x0400;
      m_cartridge
          ->chr()[(m_chrOffsets[slot] + offset) % m_cartridge->chr().size()] =
          value;
    }
  }
}

void MMC3::tick() {}
