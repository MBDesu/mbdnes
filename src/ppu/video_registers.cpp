#include "ppu/video_registers.h"
#include "cpu/cpu.h"
#include "ppu/ppu.h"

#include <cmath>

// ======== PPUCtrl ========
void PPUCtrl::onWrite(u8 value) {
  reg.raw = value;
  m_ppu->m_loopy.onPPUCtrlWrite(value);
}

// ======== PPUMask ========
u32 PPUMask::transform(u32 color) {
  u8 r = (color >> 0) & 0xff;
  u8 g = (color >> 8) & 0xff;
  u8 b = (color >> 16) & 0xff;

  if (reg.bits.grayscale) {
    r = g = b = static_cast<u8>(std::floor((r + g + b) / 3.0));
  }

  const bool emphasizeRed = reg.bits.emphasizeRed;
  const bool emphasizeGreen = reg.bits.emphasizeGreen;
  const bool emphasizeBlue = reg.bits.emphasizeBlue;
  const bool all = emphasizeRed && emphasizeBlue && emphasizeGreen;

  if (all || !emphasizeRed)
    r = static_cast<u8>(std::floor(r * 0.75));
  if (all || !emphasizeGreen)
    g = static_cast<u8>(std::floor(g * 0.75));
  if (all || !emphasizeBlue)
    b = static_cast<u8>(std::floor(b * 0.75));

  return 0xff000000 | (r << 0) | (g << 8) | (b << 16);
}

// ======== PPUStatus ========
u8 PPUStatus::onRead() {
  auto value = getValue();
  m_ppu->m_registers.m_ppuStatus.setVBlank(false);
  m_ppu->m_loopy.onPPUStatusRead();
  return value;
}

// ======== OAMData ========
u8 OAMData::onRead() {
  return m_ppu->memory().m_oamRam[m_ppu->m_registers.m_oamAddr.getValue()];
}

void OAMData::onWrite(u8 value) {
  const u8 oamAddress = m_ppu->m_registers.m_oamAddr.getValue();
  m_ppu->memory().m_oamRam[oamAddress] = value;
  m_ppu->m_registers.m_oamAddr.onWrite(oamAddress + 1);
}

// ======== PPUScroll ========
void PPUScroll::onWrite(u8 value) {
  bool currentLatch = m_ppu->m_loopy.m_latch;
  m_ppu->m_loopy.onPPUScrollWrite(value);
}

// ======== PPUAddr ========
void PPUAddr::onWrite(u8 value) { m_ppu->m_loopy.onPPUAddrWrite(value); }

u16 PPUAddr::getAddress() { return m_ppu->m_loopy.m_vAddress.getValue(); }

void PPUAddr::setAddress(u16 address) {
  m_ppu->m_loopy.m_vAddress.setValue(address);
}

// ======== PPUData ========
u8 PPUData::onRead() {
  auto data = m_buffer;
  const u16 ppuDataAddr = m_ppu->m_registers.m_ppuAddr.getAddress();

  m_buffer = m_ppu->memory().read(ppuDataAddr);

  if (ppuDataAddr >= 0x3f00 && ppuDataAddr <= 0x3fff) {
    data = m_buffer;
  }

  incrementAddress();
  return data;
}

void PPUData::onWrite(u8 value) {
  m_ppu->memory().write(m_ppu->m_registers.m_ppuAddr.getAddress(), value);
  incrementAddress();
}

void PPUData::incrementAddress() {
  u16 addr = m_ppu->m_registers.m_ppuAddr.getAddress();

  if (m_ppu->m_registers.m_ppuCtrl.reg.bits.vramAddressIncrement == 1) {
    addr += 32;
  } else {
    addr += 1;
  }

  m_ppu->m_registers.m_ppuAddr.setAddress(addr & 0x7fff);
}

// ======== OAMDma ========
void OAMDma::onWrite(u8 value) {
  u8 oamAddress = m_ppu->m_registers.m_oamAddr.getValue();
  for (auto i = 0; i < 256; i++) {

    const u16 address = (value << 8) | (i & 0xff);
    m_ppu->memory().m_oamRam[oamAddress++] =
        m_ppu->m_cpu.memory().read(address);
  }
  m_ppu->m_registers.m_oamAddr.onWrite(oamAddress);
  m_ppu->m_cpu.m_extraCycles += (m_ppu->m_cpu.m_cycle % 2 == 0) ? 513 : 514;
}
