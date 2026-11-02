#pragma once
#ifndef MODULE_CPU_CPU_MEMORY_H
#define MODULE_CPU_CPU_MEMORY_H

#include "globals.h"

#include <memory>
#include <vector>

class APU;    // forward declaration
class PPU;    // forward declaration
class Mapper; // forward declaration

class CPUMemory {
public:
  u8 m_p1InputState = 0;
  u8 m_p2InputState = 0;
  std::shared_ptr<PPU> m_ppu;
  std::shared_ptr<APU> m_apu;

  CPUMemory() : m_ram{std::vector<u8>(2048, 0)} {};

  std::shared_ptr<Mapper> getMapper() { return m_mapper; };
  u8 read(const u16 address);
  u16 read16(const u16 address);
  void write(const u16 address, const u8 value);
  void onLoad(std::shared_ptr<PPU> ppu, std::shared_ptr<APU> apu,
              std::shared_ptr<Mapper> mapper /*, controllers*/);

private:
  u8 m_p1Shift = 0;
  u8 m_p2Shift = 0;
  bool m_strobeInput = false;
  std::vector<u8> m_ram;
  std::shared_ptr<Mapper> m_mapper;
};

#endif // MODULE_CPU_CPU_MEMORY_H
