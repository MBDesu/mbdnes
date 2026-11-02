#ifndef MODULE_PPU_PPU_H
#define MODULE_PPU_PPU_H

#pragma once

#include "cpu/interrupts.h"
#include "loopy.h"
#include "mapper.h"
#include "ppu/ppu_memory.h"
#include "sprite.h"
#include "video_registers.h"

#include <functional>

class CPU; // forward declaration

class BackgroundRenderer {
public:
  PPU *m_ppu;

  BackgroundRenderer(PPU *ppu) : m_ppu{ppu} {}
  void renderScanline();

private:
  u8 getBackgroundPaletteId(u8 nameTableId, u16 x, u16 y) const;
};

class Tile {
public:
  PPU *m_ppu;
  u8 m_lowRow, m_highRow;
  u16 m_tableAddress, m_lowPlaneAddress, m_highPlaneAddress;

  Tile(PPU *ppu, u8 patternTableId, u8 tileId, u8 y);

  u8 getColorIndex(u8 x) {
    const u8 pos = static_cast<u8>(7 - x);
    const u8 bit1 = static_cast<u8>((m_lowRow >> pos) & 1);
    const u8 bit2 = static_cast<u8>((m_highRow >> pos) & 1);
    return static_cast<u8>((bit2 << 1) | bit1);
  }
};

struct SpriteBuffer {
  u8 x;
  Sprite sprite;
  u32 color;
};

class SpriteRenderer {
public:
  PPU *m_ppu;

  explicit SpriteRenderer(PPU *ppu) : m_ppu{ppu} {}
  virtual ~SpriteRenderer() = default;

  void renderScanline();

private:
  Sprite createSprite(u8 id);
  std::shared_ptr<std::vector<Sprite>> evaluate();
  std::shared_ptr<std::vector<SpriteBuffer>>
  render(std::shared_ptr<std::vector<Sprite>> sprites);
  void draw(std::shared_ptr<std::vector<SpriteBuffer>> buffer);
};

class PPU {
public:
  int m_cycle;
  int m_scanline;
  int m_frame;
  int m_sprite0HitCycleTarget{-1};

  std::vector<u8> m_colorIndices;
  std::vector<u32> m_frameBuffer;

  CPU &m_cpu;
  BackgroundRenderer m_backgroundRenderer;
  SpriteRenderer m_spriteRenderer;
  LoopyRegister m_loopy;
  VideoRegisters m_registers;

  PPU(CPU &cpu);
  ~PPU() = default;

  PPUMemory &memory() { return m_memory; }

  void onLoad(std::shared_ptr<Mapper> mapper) { m_mapper = mapper; }
  void registerCallbacks(std::function<void(std::vector<u32> &)> onFrame,
                         std::function<void(Interrupt)> onIntr);
  u32 getColor(u8 paletteId, u8 colorIndex);
  void plot(u16 x, u16 y, u32 color);
  void plotBG(u16 x, u16 y, u32 color, u8 colorIndex = 0);
  bool isBackgroundPixelOpaque(u16 x, u16 y);

  template <typename FrameCallback, typename InterruptCallback>
  void step(FrameCallback &&onFrame, InterruptCallback &&onInterrupt) {
    if (m_scanline == -1) {
      onPreLine();
    } else if (m_scanline < 240) {
      onVisibleLine();
    } else if (m_scanline == 241) {
      onVBlankLine(onInterrupt);
    }
    m_cycle++;
    if (m_mapper)
      m_mapper->tick();
    if (m_cycle >= 341) {
      m_cycle = 0;
      m_scanline++;
      if (m_scanline >= 261) {
        m_scanline = -1;
        m_frame++;

        onFrame(m_frameBuffer);
      }
    }
  }

private:
  PPUMemory m_memory;
  std::shared_ptr<Mapper> m_mapper;

  void onPreLine();
  void onVisibleLine();
  void onVBlankLine(std::function<void(Interrupt)> onInterrupt);
};

#endif // MODULE_PPU_PPU_H
