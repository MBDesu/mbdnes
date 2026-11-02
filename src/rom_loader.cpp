#include "rom_loader.h"
#include "globals.h"

#include <fstream>
#include <memory>
#include <vector>

namespace ROMLoader {
std::shared_ptr<Cartridge> loadFromFile(std::string_view filepath) {
  std::ifstream file(filepath.data(), std::ios::binary | std::ios::ate);
  if (!file.is_open()) {
    throw std::runtime_error("Error loading ROM: Couldn't open file path");
  }
  std::streamsize fileSize = file.tellg();
  if (fileSize < 16)
    throw std::runtime_error(
        "Error loading ROM: file is too small to contain an iNES header");
  file.seekg(0, std::ios::beg);

  u8 rawHeader[16];
  file.read(reinterpret_cast<char *>(rawHeader), 16);

  if (rawHeader[0] != 'N' || rawHeader[1] != 'E' || rawHeader[2] != 'S' ||
      rawHeader[3] != 0x1a) {
    throw std::runtime_error("Error loading ROM: not a valid iNES header");
  }

  Header header;
  header.prgRomPages = rawHeader[4];
  header.chrRomPages = rawHeader[5];
  header.usesChrRam = (header.chrRomPages == 0);

  u8 flags6 = rawHeader[6];
  u8 flags7 = rawHeader[7];

  header.has512BytePadding = (flags6 & 0x4) != 0;
  header.hasPrgRam = (flags6 & 0x2) != 0;

  bool isNES20 = ((rawHeader[7] & 0x0c) == 0x08);
  bool isDiskDude = (rawHeader[12] != 0 || rawHeader[13] != 0 ||
                     rawHeader[14] != 0 || rawHeader[15] != 0);

  if (isNES20) {
    header.mapperId = (flags7 & 0xf0) | ((flags6 & 0xf0) >> 4);
    header.submapperId = (rawHeader[8] >> 4) & 0x0f;
  } else {
    header.submapperId = 0;

    if (isDiskDude && ((rawHeader[7] & 0x0c) == 0)) {
      header.mapperId = (flags6 & 0xf0) >> 4;
    } else {
      header.mapperId = (flags7 & 0xf0) | ((flags6 & 0xf0) >> 4);
    }
  }

  if ((flags6 & 0x8) != 0) {
    header.mirroringId = "FOUR_SCREEN";
  } else if ((flags6 & 0x1) != 0) {
    header.mirroringId = "VERTICAL";
  } else {
    header.mirroringId = "HORIZONTAL";
  }

  size_t prgSize = static_cast<size_t>(header.prgRomPages) * 16384;
  size_t chrSize = static_cast<size_t>(header.chrRomPages) * 8192;
  size_t expectedPayloadSize = prgSize + chrSize;

  if (header.has512BytePadding) {
    expectedPayloadSize += 512;
  }

  if (static_cast<size_t>(fileSize) < (16 + expectedPayloadSize)) {
    throw std::runtime_error("Error loading ROM: File size on disk is smaller "
                             "than requested by the iNES header.");
  }

  std::vector<u8> rawBytes(expectedPayloadSize);
  file.read(reinterpret_cast<char *>(rawBytes.data()), expectedPayloadSize);
  file.close();

  return std::make_unique<Cartridge>(header, std::move(rawBytes));
}
} // namespace ROMLoader
