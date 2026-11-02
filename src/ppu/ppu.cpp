#include "ppu/ppu.h"
#include "cpu/cpu.h"
#include "cpu/interrupts.h"
#include "ppu/palette.h"
#include "ppu/sprite.h"

#include <SDL2/SDL_log.h>
#include <algorithm>
#include <memory>

PPU::PPU(CPU &cpu)
    : m_registers{VideoRegisters{this}}, m_memory{PPUMemory{}}, m_cpu{cpu},
      m_cycle{0}, m_scanline{-1}, m_frame{0},
      m_frameBuffer{std::vector<u32>(256 * 240, 0)},
      m_colorIndices{std::vector<u8>(256 * 240, 0)},
      m_spriteRenderer{SpriteRenderer{this}},
      m_backgroundRenderer{BackgroundRenderer{this}} {};

u32 PPU::getColor(u8 paletteId, u8 colorIndex) {
  const u32 startAddress = 0x3f00 + paletteId * 4;
  const u8 masterColorIndex = m_memory.read(startAddress + colorIndex);
  return Palette::NES_PALETTE[masterColorIndex % 64];
}

void PPU::plot(u16 x, u16 y, u32 color) {
  m_frameBuffer[y * 256 + x] = m_registers.m_ppuMask.transform(color);
}

void PPU::plotBG(u16 x, u16 y, u32 color, u8 colorIndex) {
  m_colorIndices[y * 256 + x] = colorIndex;
  plot(x, y, color);
  if (m_registers.m_ppuMask.showBackground()) {
    m_loopy.onPlot(x);
  }
}

bool PPU::isBackgroundPixelOpaque(u16 x, u16 y) {
  return m_colorIndices[y * 256 + x] > 0;
}

void PPU::onPreLine() {
  if (m_cycle == 1) {
    m_registers.m_ppuStatus.setVBlank(false);
    if (m_scanline == -1) {
      m_registers.m_ppuStatus.setSpriteOverflow(false);
      m_registers.m_ppuStatus.setSprite0Hit(false);
    }
  }

  if (!m_registers.m_ppuMask.isRenderingEnabled())
    return;
  m_loopy.onPreline(m_cycle);
}

void PPU::onVisibleLine() {
  if (m_cycle == 0) {
    m_backgroundRenderer.renderScanline();
    m_spriteRenderer.renderScanline();
  }
  if (!m_registers.m_ppuMask.isRenderingEnabled()) {
    return;
  }
  m_loopy.onVisibleLine(m_cycle);
}

void PPU::onVBlankLine(std::function<void(Interrupt)> onInterrupt) {
  if (m_cycle == 0) {
    m_registers.m_ppuStatus.setVBlank(true);
  } else if (m_cycle == 1 && m_registers.m_ppuCtrl.generateNMIOnVBlank()) {
    onInterrupt(INTERRUPTS[Interrupts::NMI]);
  }
}

Tile::Tile(PPU *ppu, u8 patternTableId, u8 tileId, u8 y) : m_ppu(ppu) {
  m_tableAddress = 0x1000 * patternTableId;
  m_lowPlaneAddress = m_tableAddress + (tileId * 16);
  m_highPlaneAddress = m_lowPlaneAddress + 8;
  m_lowRow = ppu->memory().read(m_lowPlaneAddress + static_cast<u16>(y));
  m_highRow = ppu->memory().read(m_highPlaneAddress + static_cast<u16>(y));
};

Sprite SpriteRenderer::createSprite(u8 id) {
  const bool is8x16 = m_ppu->m_registers.m_ppuCtrl.isSprite8x16();
  const int index = id * 4;
  const u8 y = m_ppu->memory().m_oamRam[index] + 1;
  const u8 byte1 = m_ppu->memory().m_oamRam[index + 1];
  const u8 attributes = m_ppu->memory().m_oamRam[index + 2];
  const u8 x = m_ppu->memory().m_oamRam[index + 3];

  u8 topTileId, patternTableId = 0;

  if (is8x16) {
    topTileId = byte1 & 0xfe;
    patternTableId = byte1 & 0x01;
  } else {
    topTileId = byte1;
    patternTableId = m_ppu->m_registers.m_ppuCtrl.sprite8x8PatternTableId();
  }
  return Sprite{id, x, y, is8x16, patternTableId, topTileId, attributes};
}

std::shared_ptr<std::vector<Sprite>> SpriteRenderer::evaluate() {
  std::vector<Sprite> sprites = std::vector<Sprite>();

  for (auto i = 0; i < 64; i++) {
    Sprite sprite = createSprite(i);
    if (sprite.shouldRenderInScanline(m_ppu->m_scanline)) {
      if (sprites.size() < 8) {
        sprites.push_back(sprite);
      } else {
        m_ppu->m_registers.m_ppuStatus.setSpriteOverflow(true);
        break;
      }
    }
  }
  std::reverse(sprites.begin(), sprites.end());
  return std::make_shared<std::vector<Sprite>>(sprites);
}

std::shared_ptr<std::vector<SpriteBuffer>>
SpriteRenderer::render(std::shared_ptr<std::vector<Sprite>> sprites) {
  const bool showSpritesInFirst8Pixels =
      m_ppu->m_registers.m_ppuMask.showSpritesInFirst8Pixels();
  std::vector<SpriteBuffer> buffer{256};

  for (int i = 0; i < sprites->size(); i++) {
    Sprite sprite = sprites->at(i);
    auto insideY = sprite.diffY(m_ppu->m_scanline);
    const int spriteHeight =
        m_ppu->m_registers.m_ppuCtrl.isSprite8x16() ? 16 : 8;
    if (insideY < 0 || insideY >= spriteHeight)
      continue;
    auto tileInsideY = insideY % 8;
    Tile tile = Tile(m_ppu, sprite.m_patternTableId, sprite.tileIdFor(insideY),
                     sprite.flipY() ? 7 - tileInsideY : tileInsideY);
    const auto paletteId = sprite.paletteId() + 4;
    std::array<u32, 4> paletteColors = {
        m_ppu->getColor(paletteId, 0),
        m_ppu->getColor(paletteId, 1),
        m_ppu->getColor(paletteId, 2),
        m_ppu->getColor(paletteId, 3),
    };

    for (auto insideX = 0; insideX < 8; insideX++) {
      const auto colorIndex = tile.getColorIndex(insideX);
      if (colorIndex > 0) {
        const int calculatedX =
            sprite.flipX() ? 7 - insideX + sprite.m_x : insideX + sprite.m_x;

        if (calculatedX >= 256)
          continue;

        const u8 x = static_cast<u8>(calculatedX);

        if (!showSpritesInFirst8Pixels && x < 8) {
          continue;
        }
        const auto color = paletteColors[colorIndex];
        buffer[x] = {.x = x, .sprite = sprite, .color = color};
        if (buffer[x].sprite.m_id == 0 &&
            m_ppu->isBackgroundPixelOpaque(x, m_ppu->m_scanline) &&
            m_ppu->m_registers.m_ppuMask.showBackground()) {
          // m_ppu->m_registers.m_ppuStatus.setSprite0Hit(true);
          m_ppu->m_sprite0HitCycleTarget = x;
          // SDL_LogDebug(SDL_LOG_CATEGORY_VIDEO,
          //              "x = %d; m_scanline = %d; m_cycle = %d", x,
          //              m_ppu->m_scanline, m_ppu->m_cycle);
        }
      }
    }
  }
  return std::make_shared<std::vector<SpriteBuffer>>(buffer);
}

void SpriteRenderer::draw(std::shared_ptr<std::vector<SpriteBuffer>> buffer) {
  const auto y = m_ppu->m_scanline;
  for (auto i = 0; i < buffer->size(); i++) {
    const SpriteBuffer item = buffer->at(i);
    if (item.color > 0) {
      const auto isInFront = item.sprite.isInFrontOfBackground();
      const auto isBGOpaque = m_ppu->isBackgroundPixelOpaque(item.x, y);
      if (isInFront || !isBGOpaque) {
        m_ppu->plot(item.x, y, item.color);
      }
    }
  }
}

void SpriteRenderer::renderScanline() {
  if (!m_ppu->m_registers.m_ppuMask.showSprites())
    return;
  const u8 spritePatternTableId =
      m_ppu->m_registers.m_ppuCtrl.sprite8x8PatternTableId();
  u16 simulatedSpriteAddr = spritePatternTableId * 0x1000;
  m_ppu->m_registers.m_ppuData.m_ppu->memory().read(simulatedSpriteAddr);
  const auto sprites = evaluate();
  const auto buffer = render(sprites);
  draw(buffer);
};

void BackgroundRenderer::renderScanline() {
  const auto y = m_ppu->m_scanline;

  LoopyRegister loopySnapshot = m_ppu->m_loopy;

  const u8 patternTableId = BIT(m_ppu->m_registers.m_ppuCtrl.getValue(), 4);

  for (auto x = 0; x < 256;) {
    if (!m_ppu->m_registers.m_ppuMask.showBackground() ||
        (!m_ppu->m_registers.m_ppuMask.showBackgroundInFirst8Pixels() &&
         x < 8)) {
      m_ppu->plotBG(x++, y, m_ppu->getColor(0, 0), 0);
      continue;
    }

    const u16 tileX = loopySnapshot.m_vAddress.m_coarseX;
    const u16 tileY = loopySnapshot.m_vAddress.m_coarseY;
    const u8 nameTableId = loopySnapshot.m_vAddress.m_nameTableId;
    const u8 fineY = loopySnapshot.m_vAddress.m_fineY;
    const u8 fineX = loopySnapshot.m_fineX;
    const u16 tileIndex = tileY * 32 + tileX;
    const u8 tileId =
        m_ppu->memory().read(0x2000 + tileIndex + (nameTableId * 1024));
    const u8 metablockX = tileX / 4;
    const u8 metablockY = tileY / 4;
    const u8 attributeByte = m_ppu->memory().read(
        0x2000 + (1024 * nameTableId) + 960 + (metablockY * 8 + metablockX));
    const u8 quadrantX = (tileX % 4) / 2;
    const u8 quadrantY = (tileY % 4) / 2;
    const u8 shiftAmount = (quadrantY * 2 + quadrantX) * 2;
    const u8 paletteId = (attributeByte >> shiftAmount) & 0x03;

    u16 simulatedBgAddr = (patternTableId * 0x1000) + (tileId * 16) + fineY;
    m_ppu->memory().read(simulatedBgAddr);

    Tile tile = Tile(m_ppu, patternTableId, tileId, fineY);
    const int tileStartX = (x + fineX) & 7;
    const int tilePixels = std::min(8 - tileStartX, 256 - x);
    for (int xx = 0; xx < tilePixels; xx++) {
      const auto colorIndex = tile.getColorIndex(tileStartX + xx);
      const auto color = colorIndex > 0 ? m_ppu->getColor(paletteId, colorIndex)
                                        : m_ppu->getColor(0, 0);
      m_ppu->plotBG(x + xx, y, color, colorIndex);
    }
    loopySnapshot.m_vAddress.incrementX();
    x += tilePixels;
  }
}

u8 BackgroundRenderer::getBackgroundPaletteId(u8 nameTableId, u16 x,
                                              u16 y) const {
  const u8 metablockX = x / 32;
  const u8 metablockY = y / 32;
  const u8 metablockIndex = metablockY * 8 + metablockX;
  const u8 block = m_ppu->memory().read(0x2000 + (1024 * nameTableId) + 960 +
                                        metablockIndex);
  const u8 blockX = (x % 32) / 16;
  const u8 blockY = (y % 32) / 16;
  const u8 blockIndex = blockY * 2 + blockX;
  return static_cast<u8>(BITS(block, blockIndex * 2, 2) & 3);
}
