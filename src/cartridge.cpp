#include "cartridge.h"

Cartridge::Cartridge(Header header, std::vector<u8> bytes)
    : m_bytes{bytes}, m_header{header} {
  auto prgPos = m_header.has512BytePadding ? 512 : 0;
  auto prgSize = m_header.prgRomPages * 16384;
  m_prgBytes.assign(m_bytes.begin() + prgPos,
                    m_bytes.begin() + prgPos + prgSize);
  if (m_header.usesChrRam || m_header.chrRomPages == 0) {
    m_chrBytes.resize(8192, 0);
  } else {
    auto chrPos = prgPos + prgSize;
    auto chrSize = m_header.chrRomPages * 8192;
    m_chrBytes.assign(m_bytes.begin() + chrPos,
                      m_bytes.begin() + chrPos + chrSize);
  }
}

std::vector<u8> &Cartridge::prg() { return m_prgBytes; }

std::vector<u8> &Cartridge::chr() { return m_chrBytes; }
