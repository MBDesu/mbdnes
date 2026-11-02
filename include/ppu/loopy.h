#pragma once
#ifndef MODULE_PPU_LOOPY_H
#define MODULE_PPU_LOOPY_H

#include "globals.h"
#include "lib/byte.h"

#include <array>

constexpr u8 LOOPY_ADDR_COARSE_X_OFFSET = 0;
constexpr u8 LOOPY_ADDR_COARSE_X_MASK = 0b11111;
constexpr u8 LOOPY_ADDR_COARSE_Y_OFFSET = 5;
constexpr u8 LOOPY_ADDR_COARSE_Y_MASK = 0b11111;
constexpr u8 LOOPY_ADDR_BASE_NAME_TABLE_ID_OFFSET = 10;
constexpr u8 LOOPY_ADDR_BASE_NAME_TABLE_ID_MASK = 0b11;
constexpr u8 LOOPY_ADDR_FINE_Y_OFFSET = 12;
constexpr u8 LOOPY_ADDR_FINE_Y_MASK = 0b111;
constexpr std::array<s8, 4> NAME_TABLE_OFFSETS{1, -1, 1, -1};
constexpr u8 TILE_SIZE_PIXELS = 8;
constexpr u8 SCREEN_WIDTH = 256;

class LoopyAddress {
public:
  u8 m_coarseX;
  u8 m_coarseY;
  u8 m_nameTableId;
  u8 m_fineY;

  LoopyAddress() : m_coarseX{0}, m_coarseY{0}, m_nameTableId{0}, m_fineY{0} {};
  virtual ~LoopyAddress() = default;

  void incrementX() {
    if (m_coarseX == 31) {
      m_coarseX = 0;
      switchHorizontalNameTable();
    } else {
      m_coarseX++;
    }
  }

  void incrementY() {
    if (m_fineY < 7) {
      m_fineY++;
    } else {
      m_fineY = 0;

      if (m_coarseY == 29) {
        m_coarseY = 0;
        switchVerticalNameTable();
      } else if (m_coarseY == 31) {
        m_coarseY = 0;
      } else {
        m_coarseY++;
      }
    }
  }

  u16 toNumber() {
    return ((m_coarseX << LOOPY_ADDR_COARSE_X_OFFSET) |
            (m_coarseY << LOOPY_ADDR_COARSE_Y_OFFSET) |
            (m_nameTableId << LOOPY_ADDR_BASE_NAME_TABLE_ID_OFFSET) |
            (m_fineY << LOOPY_ADDR_FINE_Y_OFFSET));
  }

  u16 getValue() { return toNumber() & 0x3fff; }

  void setValue(u16 number) {
    m_coarseX =
        (number >> LOOPY_ADDR_COARSE_X_OFFSET) & LOOPY_ADDR_COARSE_X_MASK;
    m_coarseY =
        (number >> LOOPY_ADDR_COARSE_Y_OFFSET) & LOOPY_ADDR_COARSE_Y_MASK;
    m_nameTableId = (number >> LOOPY_ADDR_BASE_NAME_TABLE_ID_OFFSET) &
                    LOOPY_ADDR_BASE_NAME_TABLE_ID_MASK;
    m_fineY = (number >> LOOPY_ADDR_FINE_Y_OFFSET) & LOOPY_ADDR_FINE_Y_MASK;
  }

private:
  void switchHorizontalNameTable() { m_nameTableId ^= 0b1; }
  void switchVerticalNameTable() { m_nameTableId ^= 0b10; }
};

class LoopyRegister {
public:
  LoopyAddress m_vAddress;
  LoopyAddress m_tAddress;
  u8 m_fineX;
  bool m_latch;

  LoopyRegister()
      : m_vAddress{LoopyAddress{}}, m_tAddress{LoopyAddress{}}, m_fineX{0},
        m_latch{false} {};

  u16 scrolledX(u16 x) {
    return (m_vAddress.m_coarseX * TILE_SIZE_PIXELS) + m_fineX +
           (x % TILE_SIZE_PIXELS);
  }

  u16 scrolledY() {
    return m_vAddress.m_coarseY * TILE_SIZE_PIXELS + m_vAddress.m_fineY;
  }

  u8 nameTableId(u16 scrolledX) const {
    u16 horizontalPages = scrolledX / 256;
    u8 horizontalBit = horizontalPages & 1;
    return (m_vAddress.m_nameTableId & 0b10) | horizontalBit;
  }

  void onPPUCtrlWrite(u8 value) {
    m_tAddress.m_nameTableId = BITS(value, 0, 2);
  }

  void onPPUStatusRead() { m_latch = false; }

  void onPPUScrollWrite(u8 value) {
    if (!m_latch) {
      m_tAddress.m_coarseX = static_cast<u8>(BITS(value, 3, 5));
      m_fineX = static_cast<u8>(BITS(value, 0, 3));
    } else {
      m_tAddress.m_coarseY = static_cast<u8>(BITS(value, 3, 5));
      m_tAddress.m_fineY = static_cast<u8>(BITS(value, 0, 3));
    }
    m_latch = !m_latch;
  }

  void onPPUAddrWrite(u8 value) {
    if (!m_latch) {
      auto number = m_tAddress.toNumber();
      u8 high = (number & 0xff00) >> 8;
      high = (high & 0xc0) | (value & 0x3f);
      high &= 0xbf; // 0b10111111
      number = (high << 8) | (number & 0x00ff);
      m_tAddress.setValue(number);
    } else {
      auto number = m_tAddress.toNumber();
      number = (number & 0xff00) | value;
      m_tAddress.setValue(number);
      m_vAddress.setValue(number);
    }
    m_latch = !m_latch;
  }

  void onPreline(int cycle) {
    if (cycle >= 280 && cycle <= 304)
      copyY();
    onLine(cycle);
  }

  void onVisibleLine(int cycle) { onLine(cycle); }

  void onPlot(u16 x) {
    const u16 cycle = x + 1;
    if (cycle >= 8 && cycle <= 256 && cycle % 8 == 0) {
      m_vAddress.incrementX();
    }
  }

private:
  void copyX() {
    const auto v = m_vAddress.toNumber();
    const auto t = m_tAddress.toNumber();
    m_vAddress.setValue((v & 0b111101111100000) | (t & 0b000010000011111));
  }

  void copyY() {
    const auto v = m_vAddress.toNumber();
    const auto t = m_tAddress.toNumber();
    m_vAddress.setValue((v & 0b000010000011111) | (t & 0b111101111100000));
  }

  void onLine(int cycle) {
    if (cycle == 256)
      m_vAddress.incrementY();
    if (cycle == 257)
      copyX();
  }
};

#endif // MODULE_PPU_LOOPY_H
