#pragma once
#ifndef MODULE_CARTRIDGE_H
#define MODULE_CARTRIDGE_H

#include "globals.h"

#include <string_view>
#include <vector>

struct Header {
  u8 prgRomPages = 2;
  u8 chrRomPages = 1;
  bool usesChrRam = false;
  bool has512BytePadding = false;
  bool hasPrgRam = false;
  std::string_view mirroringId = "VERTICAL";
  u8 mapperId = 4;
  u8 submapperId = 0;
};

class Cartridge {
public:
  Cartridge(const Cartridge &) = default;
  Cartridge(Cartridge &&) = delete;
  Cartridge &operator=(const Cartridge &) = default;
  Cartridge &operator=(Cartridge &&) = delete;
  Cartridge(Header header, std::vector<u8> bytes);
  ~Cartridge() = default;

  std::vector<u8> &prg();
  std::vector<u8> &chr();

  Header m_header;

private:
  std::vector<u8> m_bytes;
  std::vector<u8> m_prgBytes;
  std::vector<u8> m_chrBytes;
};

#endif
