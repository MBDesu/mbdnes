#pragma once
#ifndef MODULE_MAPPERS_0_NROM_H
#define MODULE_MAPPERS_0_NROM_H

#include "mapper.h"

class NROM : public Mapper {
public:
  using Mapper::Mapper;
  void onLoad() override { return; };
  u8 cpuRead(const u16 address) override;
  void cpuWrite(const u16 address, const u8 value) override {}; // unused
  u8 ppuRead(const u16 address) override { return $getChrPage(0)[address]; };
  void ppuWrite(const u16 address, const u8 value) override;
  void tick() override {};
};

#endif // MODULE_MAPPERS_0_NROM_H
