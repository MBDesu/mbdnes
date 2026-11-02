#ifndef MODULE_PPU_VIDEO_REGISTERS_H
#define MODULE_PPU_VIDEO_REGISTERS_H

#pragma once

#include "globals.h"

class PPU; // forward declaration

union PPUCtrlRegister {
  u8 raw;
  struct {
    u8 baseNameTableAddress : 2;     // Bits 0-1: ($2000, $2400, $2800, $2C00)
    u8 vramAddressIncrement : 1;     // Bit 2: 0 = add 1, 1 = add 32
    u8 spritePatternTableId : 1;     // Bit 3: Sprite 8x8 pattern table address
    u8 backgroundPatternTableId : 1; // Bit 4: BG pattern table address
    u8 spriteSize : 1;               // Bit 5: 0 = 8x8, 1 = 8x16
    u8 ppuMasterSlaveSelect : 1;     // Bit 6: PPU master/slave (unused)
    u8 generateNMIOnVBlank : 1;      // Bit 7: 0 = off, 1 = on
  } bits;
};

union PPUMaskRegister {
  u8 raw;
  struct {
    u8 grayscale : 1;             // Bit 0: 0 = color, 1 = monochrome
    u8 showBackgroundLeft8Px : 1; // Bit 1: 1 = show BG in leftmost 8px
    u8 showSpritesLeft8Px : 1;    // Bit 2: 1 = show sprites in leftmost 8px
    u8 showBackground : 1;        // Bit 3: 1 = render background
    u8 showSprites : 1;           // Bit 4: 1 = render sprites
    u8 emphasizeRed : 1;          // Bit 5: Color emphasis
    u8 emphasizeGreen : 1;        // Bit 6: Color emphasis
    u8 emphasizeBlue : 1;         // Bit 7: Color emphasis
  } bits;
};

union PPUStatusRegister {
  u8 raw;
  struct {
    u8 residual : 5;           // Bits 0-4: Stale PPU data bus noise
    u8 spriteOverflow : 1;     // Bit 5: Sprite overflow flag
    u8 sprite0Hit : 1;         // Bit 6: Sprite 0 Hit flag
    u8 isInVBlankInterval : 1; // Bit 7: Vertical blank flag
  } bits;
};

struct PPUCtrl {
  PPUCtrlRegister reg{0};
  PPU *m_ppu;

  explicit PPUCtrl(PPU *ppu) : m_ppu(ppu) {}
  void onWrite(u8 value);
  u8 getValue() const { return reg.raw; }

  inline bool generateNMIOnVBlank() const {
    return reg.bits.generateNMIOnVBlank;
  }
  inline bool isSprite8x16() const { return reg.bits.spriteSize; }
  inline u8 backgroundPatternTableId() const {
    return reg.bits.backgroundPatternTableId;
  }
  inline u8 sprite8x8PatternTableId() const {
    return reg.bits.spritePatternTableId;
  }
  inline bool vramAddressIncrement32() const {
    return reg.bits.vramAddressIncrement;
  }
};

struct PPUMask {
  PPUMaskRegister reg{0};
  PPU *m_ppu;

  explicit PPUMask(PPU *ppu) : m_ppu(ppu) {}
  void onWrite(u8 value) { reg.raw = value; }
  u8 getValue() const { return reg.raw; }
  u32 transform(u32 color);

  inline bool showBackground() const { return reg.bits.showBackground; }
  inline bool showSprites() const { return reg.bits.showSprites; }
  inline bool showBackgroundInFirst8Pixels() const {
    return reg.bits.showBackgroundLeft8Px;
  }
  inline bool showSpritesInFirst8Pixels() const {
    return reg.bits.showSpritesLeft8Px;
  }
  inline bool grayscale() const { return reg.bits.grayscale; }

  inline bool isRenderingEnabled() const {
    return reg.bits.showBackground || reg.bits.showSprites;
  }
};

struct PPUStatus {
  PPUStatusRegister reg{0};
  PPU *m_ppu;

  explicit PPUStatus(PPU *ppu) : m_ppu(ppu) { reg.raw = 0x80; }
  u8 onRead();
  u8 getValue() const { return reg.raw; }
  void setValue(u8 val) { reg.raw = val; }

  inline bool isInVBlank() const { return reg.bits.isInVBlankInterval; }
  inline bool isSprite0Hit() const { return reg.bits.sprite0Hit; }
  inline bool isSpriteOverflow() const { return reg.bits.spriteOverflow; }

  inline void setVBlank(bool active) {
    reg.bits.isInVBlankInterval = active ? 1 : 0;
  }
  inline void setSprite0Hit(bool active) { reg.bits.sprite0Hit = active; }
  inline void setSpriteOverflow(bool active) {
    reg.bits.spriteOverflow = active ? 1 : 0;
  }
};

struct OAMAddr {
  u8 value = 0;
  PPU *m_ppu;

  explicit OAMAddr(PPU *ppu) : m_ppu(ppu) {}
  void onWrite(u8 val) { value = val; }
  u8 getValue() const { return value; }
};

struct OAMData {
  PPU *m_ppu;

  explicit OAMData(PPU *ppu) : m_ppu(ppu) {}
  u8 onRead();
  void onWrite(u8 value);
};

struct PPUScroll {
  PPU *m_ppu;

  explicit PPUScroll(PPU *ppu) : m_ppu(ppu) {}
  void onWrite(u8 value);
};

struct PPUAddr {
  PPU *m_ppu;

  explicit PPUAddr(PPU *ppu) : m_ppu(ppu) {}
  void onWrite(u8 value);
  u16 getAddress();
  void setAddress(u16 address);
};

struct PPUData {
  u8 m_buffer = 0;
  PPU *m_ppu;

  explicit PPUData(PPU *ppu) : m_ppu(ppu) {}
  u8 onRead();
  void onWrite(u8 value);

private:
  void incrementAddress();
};

struct OAMDma {
  PPU *m_ppu;

  explicit OAMDma(PPU *ppu) : m_ppu(ppu) {}
  void onWrite(u8 value);
};

class VideoRegisters {
public:
  PPUCtrl m_ppuCtrl;
  PPUMask m_ppuMask;
  PPUStatus m_ppuStatus;
  OAMAddr m_oamAddr;
  OAMData m_oamData;
  PPUScroll m_ppuScroll;
  PPUAddr m_ppuAddr;
  PPUData m_ppuData;
  OAMDma m_oamDma;

  explicit VideoRegisters(PPU *ppu)
      : m_ppuCtrl{ppu}, m_ppuMask{ppu}, m_ppuStatus{ppu}, m_oamAddr{ppu},
        m_oamData{ppu}, m_ppuScroll{ppu}, m_ppuAddr{ppu}, m_ppuData{ppu},
        m_oamDma{ppu} {}

  inline u8 read(u16 address) {
    switch (address) {
    case 0x2002:
      return m_ppuStatus.onRead();
    case 0x2004:
      return m_oamData.onRead();
    case 0x2007:
      return m_ppuData.onRead();
    default:
      return 0;
    }
  }

  inline void write(u16 address, u8 value) {
    switch (address) {
    case 0x2000:
      m_ppuCtrl.onWrite(value);
      break;
    case 0x2001:
      m_ppuMask.onWrite(value);
      break;
    case 0x2003:
      m_oamAddr.onWrite(value);
      break;
    case 0x2004:
      m_oamData.onWrite(value);
      break;
    case 0x2005:
      m_ppuScroll.onWrite(value);
      break;
    case 0x2006:
      m_ppuAddr.onWrite(value);
      break;
    case 0x2007:
      m_ppuData.onWrite(value);
      break;
    case 0x4014:
      m_oamDma.onWrite(value);
      break;
    default:
      break;
    }
  }
};

#endif // MODULE_PPU_VIDEO_REGISTERS_H
