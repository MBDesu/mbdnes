#include "cpu/cpu_memory.h"
#include "apu/apu.h"
#include "mapper.h"
#include "ppu/ppu.h"

void CPUMemory::onLoad(std::shared_ptr<PPU> ppu, std::shared_ptr<APU> apu,
                       std::shared_ptr<Mapper> mapper) {
  m_apu = apu;
  m_ppu = ppu;
  m_mapper = mapper;
}

u8 CPUMemory::read(const u16 address) {
  // WRAM
  if (address >= 0 && address < 0x800) {
    return m_ram[address];
  }

  // Mirrors of $0000-$07FF
  if (address >= 0x800 && address < 0x2000) {
    return read((address - 0x800) % 0x800);
  }

  // PPU registers
  if (address >= 0x2000 && address < 0x2008) {
    if (m_ppu != nullptr) {
      return m_ppu->m_registers.read(address);
    }
    return 0;
  }

  // Mirrors of $2000-2007
  if (address >= 0x2008 && address <= 0x3fff)
    return read(0x2000 + ((address - 0x2008) % 0x8));

  // APU registers
  if (address >= 0x4000 && address <= 0x4013) {
    return m_apu->m_registers.read(address);
    return 0;
  }

  // PPU's OAMDMA register
  if (address == 0x4014)
    return m_ppu->m_registers.read(address);

  // APUStatus register
  if (address == 0x4015)
    return m_apu->m_registers.read(address);

  // Controller port 1
  if (address == 0x4016) {
    u8 value = m_p1Shift & 0x01;
    m_p1Shift >>= 1;
    return value | 0x40;
  }

  // Controller port 2
  if (address == 0x4017) {
    u8 value = m_p2Shift & 0x01;
    m_p2Shift >>= 1;
    return value | 0x40;
  }

  // Cartridge space (PRG-ROM, mapper, etc.)
  if (address >= 0x4020) { // u16 is always < 0x10000
    if (m_mapper != nullptr) {
      return m_mapper->cpuRead(address);
    }
    return 0;
  }

  return 0;
}

void CPUMemory::write(const u16 address, const u8 value) {
  // WRAM
  if (address >= 0 && address < 0x800) {
    m_ram[address] = value;
    return;
  }

  // Mirrors of $0000-$07FF
  if (address >= 0x0800 && address <= 0x1fff)
    return write(0x0000 + (address - 0x0800) % 0x0800, value);

  // PPU registers
  if (address >= 0x2000 && address <= 0x2007) {
    if (m_ppu != nullptr) {
      m_ppu->m_registers.write(address, value);
      return;
    }
  }

  // Mirrors of $2000-2007
  if (address >= 0x2008 && address <= 0x3fff) {
    write((0x2000 + (address - 0x2008) % 0x8), value);
    return;
  }

  // APU registers
  if (address >= 0x4000 && address <= 0x4013) {
    m_apu->m_registers.write(address, value);
    return;
  }

  // PPU's OAMDMA register
  if (address == 0x4014) {
    if (m_ppu != nullptr) {
      m_ppu->m_registers.write(address, value);
      return;
    }
  }

  // APUControl register
  if (address == 0x4015) {
    if (m_apu != nullptr) {
      m_apu->m_registers.write(address, value);
      return;
    }
  }

  // Controller port 1
  if (address == 0x4016) {
    m_strobeInput = value & 0x1;
    if (m_strobeInput) {
      m_p1Shift = m_p1InputState;
      m_p2Shift = m_p2InputState;
    }
    return;
  }

  // APUFrameCounter register
  if (address == 0x4017) {
    if (m_apu != nullptr) {
      m_apu->m_registers.write(address, value);
      return;
    }
  }

  // Cartridge space (PRG-ROM, mapper, etc.)
  if (address >= 0x4020) {
    if (m_mapper != nullptr) {
      m_mapper->cpuWrite(address, value);
      return;
    }
  }
}

u16 CPUMemory::read16(const u16 address) {
  u16 low = read(address);
  u16 high = read(address + 1);
  return (high << 8) | low;
}
