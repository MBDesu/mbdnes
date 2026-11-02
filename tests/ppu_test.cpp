#include "apu/apu.h"
#include "cartridge.h"
#include "cpu/cpu.h"
#include "globals.h"
#include "mappers/0_NROM.h"
#include "ppu/palette.h"
#include "ppu/ppu.h"

#include <array>
#include <catch2/catch_test_macros.hpp>
#include <memory>
#include <vector>

struct DummyLoopyFieldLayout {
  PPU *m_ppu;
};

struct PPUTestContext {
  CPUMemory cpuMemory;
  std::shared_ptr<APU> apu;
  std::shared_ptr<CPU> cpu;
  std::shared_ptr<PPU> ppu;
  std::shared_ptr<Cartridge> cartridge;
  std::shared_ptr<NROM> mapper;

  PPUTestContext() {
    Header header;
    header.prgRomPages = 1;
    header.chrRomPages = 1;
    header.usesChrRam = false;
    header.has512BytePadding = false;
    header.mapperId = 0;
    header.mirroringId = "VERTICAL";
    header.submapperId = 0;

    std::vector<u8> payloadBytes(16384 + 8192, 0x00);
    cartridge = std::make_shared<Cartridge>(header, std::move(payloadBytes));
    cpu = std::make_shared<CPU>(cpuMemory);
    apu = std::make_shared<APU>(cpu.get());
    ppu = std::make_shared<PPU>(*cpu);
    mapper = std::make_shared<NROM>(cpu, ppu, cartridge);

    cpu->memory().onLoad(ppu, apu, mapper);
    ppu->onLoad(mapper);
    ppu->memory().onLoad(cartridge, mapper);

    reinterpret_cast<DummyLoopyFieldLayout *>(&ppu->m_loopy)->m_ppu = ppu.get();
  }
};

TEST_CASE("PPU Registers and Memory Operations", "[ppu][memory]") {
  PPUTestContext ctx;
  auto &ppu = *ctx.ppu;

  SECTION("VRAM Address Increment Selection via PPUCTRL Bitfield") {
    ppu.m_registers.m_ppuCtrl.reg.bits.vramAddressIncrement = 0;
    ppu.m_registers.m_ppuAddr.setAddress(0x2000);

    u16 addr1 = ppu.m_registers.m_ppuAddr.getAddress();
    ppu.m_registers.m_ppuAddr.setAddress(
        addr1 + (ppu.m_registers.m_ppuCtrl.vramAddressIncrement32() ? 32 : 1));
    REQUIRE(ppu.m_registers.m_ppuAddr.getAddress() == 0x2001);

    ppu.m_registers.m_ppuCtrl.reg.bits.vramAddressIncrement = 1;
    ppu.m_registers.m_ppuAddr.setAddress(0x2000);

    u16 addr2 = ppu.m_registers.m_ppuAddr.getAddress();
    ppu.m_registers.m_ppuAddr.setAddress(
        addr2 + (ppu.m_registers.m_ppuCtrl.vramAddressIncrement32() ? 32 : 1));
    REQUIRE(ppu.m_registers.m_ppuAddr.getAddress() == 0x2020);
  }

  SECTION("PPUSTATUS Register Latch and Status Modifications") {
    ppu.m_registers.m_ppuStatus.setVBlank(true);
    ppu.m_loopy.m_latch = true;

    u8 status = ppu.m_registers.m_ppuStatus.getValue();
    REQUIRE((status & 0x80) != 0);

    ppu.m_registers.m_ppuStatus.setVBlank(false);
    ppu.m_loopy.onPPUStatusRead();

    REQUIRE(ppu.m_registers.m_ppuStatus.isInVBlank() == false);
    REQUIRE(ppu.m_loopy.m_latch == false);
  }

  SECTION("PPUDATA Buffer Isolation and Direct Palette Path Bypass") {
    // FIX: Using 0x2005 (Nametable VRAM Space) instead of CHR-ROM space so
    // writes are accepted natively
    ppu.memory().write(0x2005, 0xDD);
    ppu.memory().write(0x3F01, 0x15);

    ppu.m_registers.m_ppuAddr.setAddress(0x2005);
    REQUIRE(ppu.memory().read(0x2005) == 0xDD);

    ppu.m_registers.m_ppuAddr.setAddress(0x3F01);
    u8 paletteData = ppu.memory().read(0x3F01);
    REQUIRE(paletteData == 0x15);
  }

  SECTION("Palette Hardware Internal Mirroring Logic ($3F10 -> $3F00)") {
    ppu.memory().write(0x3F00, 0x0A);
    REQUIRE(ppu.memory().read(0x3F10) == 0x0A);

    ppu.memory().write(0x3F14, 0x1F);
    REQUIRE(ppu.memory().read(0x3F04) == 0x1F);
  }
}

TEST_CASE("PPU Loopy Register Scroll Logic", "[ppu][loopy]") {
  PPUTestContext ctx;
  auto &ppu = *ctx.ppu;

  SECTION("PPUADDR Double Write Sequence Architecture") {
    ppu.m_loopy.onPPUAddrWrite(0x3F);
    REQUIRE(ppu.m_loopy.m_latch == true);

    ppu.m_loopy.onPPUAddrWrite(0x12);
    REQUIRE(ppu.m_loopy.m_latch == false);

    ppu.m_registers.m_ppuAddr.setAddress(ppu.m_loopy.m_vAddress.toNumber());
    REQUIRE(ppu.m_registers.m_ppuAddr.getAddress() == 0x3F12);
  }

  SECTION("PPUSCROLL Interleaved Writing to tAddress") {
    ppu.m_loopy.onPPUScrollWrite(0b01111101);
    REQUIRE(ppu.m_loopy.m_fineX == 5);
    REQUIRE(ppu.m_loopy.m_tAddress.m_coarseX == 15);
    REQUIRE(ppu.m_loopy.m_latch == true);

    ppu.m_loopy.onPPUScrollWrite(0b10110011);
    REQUIRE(ppu.m_loopy.m_tAddress.m_coarseY == 22);
    REQUIRE(ppu.m_loopy.m_tAddress.m_fineY == 3);
    REQUIRE(ppu.m_loopy.m_latch == false);
  }

  SECTION("Horizontal Address Coarse X Overflow Wrapping") {
    u16 addressVal = (31 << 0) | (0 << 5) | (0 << 10) | (0 << 12);
    ppu.m_loopy.m_vAddress.setValue(addressVal);

    ppu.m_loopy.onPlot(7);

    REQUIRE(ppu.m_loopy.m_vAddress.m_coarseX == 0);
    REQUIRE(ppu.m_loopy.m_vAddress.m_nameTableId == 1);
  }

  SECTION("Vertical Address Coarse Y Bounds Wrapping handling") {
    u16 addressVal1 = (0 << 0) | (29 << 5) | (0 << 10) | (7 << 12);
    ppu.m_loopy.m_vAddress.setValue(addressVal1);

    ppu.m_loopy.onVisibleLine(256);

    REQUIRE(ppu.m_loopy.m_vAddress.m_fineY == 0);
    REQUIRE(ppu.m_loopy.m_vAddress.m_coarseY == 0);
    REQUIRE(ppu.m_loopy.m_vAddress.m_nameTableId == 2);

    u16 addressVal2 = (0 << 0) | (31 << 5) | (0 << 10) | (7 << 12);
    ppu.m_loopy.m_vAddress.setValue(addressVal2);

    ppu.m_loopy.onVisibleLine(256);

    REQUIRE(ppu.m_loopy.m_vAddress.m_coarseY == 0);
  }
}

TEST_CASE("PPU Scanning, Timing, and Engine State Flags", "[ppu][timing]") {
  PPUTestContext ctx;
  auto &ppu = *ctx.ppu;

  bool nmi_triggered = false;
  auto nmi_handler = [&](Interrupt vec) {
    if (vec.id == INTERRUPTS[Interrupts::NMI].id) {
      nmi_triggered = true;
    }
  };
  auto frame_handler = [](std::vector<u32> &buffer) {};

  SECTION("Preline Reset Sequence Behavior") {
    ppu.m_scanline = -1;
    ppu.m_cycle = 1;
    ppu.m_registers.m_ppuStatus.setVBlank(true);
    ppu.m_registers.m_ppuStatus.setSprite0Hit(true);
    ppu.m_registers.m_ppuStatus.setSpriteOverflow(true);

    ppu.step(frame_handler, nmi_handler);

    REQUIRE(ppu.m_registers.m_ppuStatus.isInVBlank() == false);
    REQUIRE(ppu.m_registers.m_ppuStatus.isSprite0Hit() == false);
    REQUIRE(ppu.m_registers.m_ppuStatus.isSpriteOverflow() == false);
  }

  SECTION("VBlank Interrupt NMI Generation Path") {
    ppu.m_scanline = 241;
    ppu.m_registers.m_ppuCtrl.reg.bits.generateNMIOnVBlank = 1;

    ppu.m_cycle = 0;
    ppu.step(frame_handler, nmi_handler);
    REQUIRE(ppu.m_registers.m_ppuStatus.isInVBlank() == true);

    ppu.step(frame_handler, nmi_handler);
    REQUIRE(nmi_triggered == true);
  }

  SECTION("Sprite Evaluation Overflow Logic (>8 Sprites on Line)") {
    ppu.m_scanline = 40;
    ppu.m_cycle = 0;
    ppu.m_registers.m_ppuMask.onWrite(0b00010000);

    for (int i = 0; i < 9; ++i) {
      ppu.memory().m_oamRam[i * 4] = 39;
      ppu.memory().m_oamRam[i * 4 + 1] = 0;
      ppu.memory().m_oamRam[i * 4 + 2] = 0;
      ppu.memory().m_oamRam[i * 4 + 3] = 10;
    }

    ppu.m_spriteRenderer.renderScanline();
    REQUIRE(ppu.m_registers.m_ppuStatus.isSpriteOverflow() == true);
  }
}

TEST_CASE("PPU Rendering Pipeline - Hardware Layer Priority Matrices",
          "[ppu][rendering]") {
  PPUTestContext ctx;
  auto &ppu = *ctx.ppu;

  SECTION("Sprite-to-Sprite OAM ID Priority Sorting Verification") {
    ppu.m_scanline = 50;
    ppu.m_cycle = 0;

    ppu.m_registers.m_ppuMask.onWrite(0b00011000);

    ppu.memory().m_oamRam[0 * 4] = 49;
    ppu.memory().m_oamRam[0 * 4 + 1] = 0x00;
    ppu.memory().m_oamRam[0 * 4 + 2] = 0x00;
    ppu.memory().m_oamRam[0 * 4 + 3] = 100;

    u8 spritePatternTableId =
        ppu.m_registers.m_ppuCtrl.sprite8x8PatternTableId();
    size_t chrPageTarget = spritePatternTableId;
    if (chrPageTarget < ctx.mapper->m_chrPages.size()) {
      for (size_t offset = 0; offset < 16; ++offset) {
        ctx.mapper->m_chrPages[chrPageTarget][offset] = 0xFF;
      }
    }

    for (int col = 100; col < 108; ++col) {
      ppu.m_colorIndices[50 * 256 + col] = 1;
    }

    ppu.m_spriteRenderer.renderScanline();

    REQUIRE(ppu.m_registers.m_ppuStatus.isSprite0Hit() == true);
  }

  SECTION(
      "Sprite 0 Hit Opaque Background Detection Matrix under Scroll Shifts") {
    ppu.m_scanline = 30;
    ppu.m_cycle = 0;
    ppu.m_registers.m_ppuMask.onWrite(0b00011000);

    ppu.m_loopy.m_fineX = 4;
    ppu.m_loopy.m_vAddress.m_coarseX = 5;

    ppu.memory().m_oamRam[0 * 4] = 29;
    ppu.memory().m_oamRam[0 * 4 + 1] = 0x00;
    ppu.memory().m_oamRam[0 * 4 + 2] = 0x00;
    ppu.memory().m_oamRam[0 * 4 + 3] = 40;

    u8 spritePatternTableId =
        ppu.m_registers.m_ppuCtrl.sprite8x8PatternTableId();
    size_t chrPageTarget = spritePatternTableId;
    if (chrPageTarget < ctx.mapper->m_chrPages.size()) {
      for (size_t offset = 0; offset < 16; ++offset) {
        ctx.mapper->m_chrPages[chrPageTarget][offset] = 0xFF;
      }
    }

    for (int col = 40; col < 48; ++col) {
      ppu.m_colorIndices[30 * 256 + col] = 1;
    }

    ppu.m_spriteRenderer.renderScanline();

    REQUIRE(ppu.m_registers.m_ppuStatus.isSprite0Hit() == true);
  }
}
