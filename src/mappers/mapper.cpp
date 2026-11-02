#include "mapper.h"
#include "mappers/0_NROM.h"
#include "mappers/1_MMC1.h"
#include "mappers/2_UxROM.h"
#include "mappers/3_CNROM.h"
#include "mappers/4_MMC3.h"

#include <fmt/format.h>
#include <memory>
#include <stdexcept>

std::shared_ptr<Mapper> createMapper(std::shared_ptr<CPU> cpu,
                                     std::shared_ptr<PPU> ppu,
                                     std::shared_ptr<Cartridge> cartridge) {
  switch (cartridge->m_header.mapperId) {
  case 0:
    return std::make_unique<NROM>(cpu, ppu, cartridge);
  case 1:
    return std::make_unique<MMC1>(cpu, ppu, cartridge);
  case 2:
    return std::make_unique<UxROM>(cpu, ppu, cartridge);
  case 3:
    return std::make_unique<CNROM>(cpu, ppu, cartridge);
  case 4:
    return std::make_unique<MMC3>(cpu, ppu, cartridge);
  default:
    throw std::runtime_error(
        fmt::format("mapper not supported: %d", cartridge->m_header.mapperId));
  }
}
