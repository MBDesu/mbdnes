#pragma once
#ifndef MODULE_MAPPER_H
#define MODULE_MAPPER_H

#include "cartridge.h"
#include "cpu/cpu.h"
#include "globals.h"

#include <cmath>
#include <memory>

class Mapper {
public:
  std::vector<std::vector<u8>> m_prgPages;
  std::vector<std::vector<u8>> m_chrPages;

  Mapper(std::shared_ptr<CPU> cpu, std::shared_ptr<PPU> ppu,
         std::shared_ptr<Cartridge> cartridge)
      : m_cartridge{cartridge}, m_cpu{cpu}, m_ppu{ppu} {
    const auto prg = m_cartridge->prg();
    const auto chr = m_cartridge->chr();
    const int totalPrgPages = std::floor(prg.size() / prgRomPageSize());
    const int totalChrPages = std::floor(prg.size() / chrRomPageSize());

    m_prgPages = std::vector<std::vector<u8>>(totalPrgPages);
    for (auto i = 0; i < totalPrgPages; i++) {
      m_prgPages[i] = getPage(prg, prgRomPageSize(), i);
    }

    m_chrPages = std::vector<std::vector<u8>>(totalChrPages);
    for (auto i = 0; i < totalChrPages; i++) {
      m_chrPages[i] = getPage(chr, chrRomPageSize(), i);
    }
    onLoad();
  }
  ~Mapper() = default;

  virtual void onLoad() { return; };
  virtual u8 cpuRead(const u16 address) = 0;
  virtual void cpuWrite(const u16 address, const u8 value) = 0;
  virtual u8 ppuRead(const u16 address) = 0;
  virtual void ppuWrite(const u16 address, const u8 value) = 0;
  virtual void tick() = 0;

  std::vector<u8> &$getPrgPage(auto page) {
    return m_prgPages[std::max(0, static_cast<int>(page) %
                                      static_cast<int>(m_prgPages.size()))];
  }

  std::vector<u8> &$getChrPage(auto page) {
    return m_chrPages[std::max(0, static_cast<int>(page) %
                                      static_cast<int>(m_chrPages.size()))];
  }

  int prgRomPageSize() { return 16 * 1024; }
  int chrRomPageSize() { return 8 * 1024; }

protected:
  std::shared_ptr<Cartridge> m_cartridge;
  std::shared_ptr<CPU> m_cpu;
  std::shared_ptr<PPU> m_ppu;

private:
  std::vector<u8> getPage(std::vector<u8> memory, auto pageSize, auto page) {
    const auto offset = page * pageSize;
    return std::vector<u8>(memory.begin() + offset,
                           memory.begin() + offset + pageSize);
  }
};

std::shared_ptr<Mapper> createMapper(std::shared_ptr<CPU> cpu,
                                     std::shared_ptr<PPU> ppu,
                                     std::shared_ptr<Cartridge> cartridge);

#endif // MODULE_MAPPER_H
