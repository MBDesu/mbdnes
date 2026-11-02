#pragma once
#ifndef MODULE_ROM_LOADER_H
#define MODULE_ROM_LOADER_H

#include "cartridge.h"

#include <memory>
#include <string_view>

namespace ROMLoader {
std::shared_ptr<Cartridge> loadFromFile(std::string_view filepath);
} // namespace ROMLoader

#endif // MODULE_ROM_LOADER_H
