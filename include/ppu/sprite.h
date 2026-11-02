#ifndef MODULE_PPU_SPRITE_H
#define MODULE_PPU_SPRITE_H

#pragma once

#include "globals.h"

class Sprite {
public:
  constexpr static u8 TILE_SIZE_PIXELS = 8;
  constexpr static u8 PALETTE_FOREGROUND_START = 4;
  constexpr static u8 SPRITE_ATTR_PALETTE_BITS_START = 0;
  constexpr static u8 SPRITE_ATTR_PALETTE_BITS_SIZE = 2;
  constexpr static u8 SPRITE_ATTR_PRIORITY_BIT = 5;
  constexpr static u8 SPRITE_ATTR_HORIZONTAL_FLIP_BIT = 6;
  constexpr static u8 SPRITE_ATTR_VERTICAL_FLIP_BIT = 7;

  int m_id;
  u8 m_x, m_y, m_patternTableId, m_tileId, m_attributes;
  bool m_is8x16;

  Sprite()
      : m_id{0}, m_x{0}, m_y{0}, m_is8x16{false}, m_patternTableId{0},
        m_tileId{0}, m_attributes{0} {}

  Sprite(u8 id, u8 x, u8 y, bool is8x16, u8 patternTableId, u8 topTileId,
         u8 attributes)
      : m_id{id}, m_x{x}, m_y{y}, m_is8x16{is8x16},
        m_patternTableId{patternTableId}, m_tileId{topTileId},
        m_attributes{attributes} {}

  u8 tileIdFor(u8 insideY) {
    int index = 0 + (insideY >= TILE_SIZE_PIXELS);
    if (m_is8x16 && flipY()) {
      if (index == 0)
        index = 1;
      else if (index == 1)
        index = 0;
    }
    return m_tileId + index;
  }

  u8 paletteId() {
    return (m_attributes >> SPRITE_ATTR_PALETTE_BITS_START) & 0b11;
  }
  int diffY(int scanline) { return scanline - (m_y /* + 1*/); }
  bool flipX() { return (m_attributes >> SPRITE_ATTR_HORIZONTAL_FLIP_BIT) & 1; }
  bool flipY() { return (m_attributes >> SPRITE_ATTR_VERTICAL_FLIP_BIT) & 1; }
  u8 height() { return m_is8x16 ? 16 : 8; }

  bool shouldRenderInScanline(int scanline) {
    const int l_diffY = diffY(scanline);
    return l_diffY >= 0 && l_diffY < height();
  }

  bool isInFrontOfBackground() const {
    return ((m_attributes >> SPRITE_ATTR_PRIORITY_BIT) & 1) == 0;
  }
};

#endif // MODULE_PPU_SPRITE_H
