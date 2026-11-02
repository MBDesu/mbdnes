
#include "apu/dpcm.h"
#include "apu/apu.h"
#include "apu/audio_registers.h"
#include "apu/dmc_channel.h"
#include "apu/dpcm_periods.h"
#include "cpu/cpu.h"
#include <algorithm>

DPCM::DPCM(DMCChannel *dmcChannel) : m_dmcChannel(dmcChannel) {}

CPU *DPCM::getCPU() const { return m_dmcChannel->getCPU(); }

DMCRegisters &DPCM::getRegisters() const {
  return m_dmcChannel->getRegisters();
}

void DPCM::update() {
  if (m_startFlag) {
    u16 period = dpcmPeriods[getRegisters().control.dpcmPeriodId];

    m_startFlag = false;
    m_isActive = true;
    m_cursorByte = -1;
    m_cursorBit = 0;
    m_dividerPeriod = period;
    m_dividerCount = period - 1;

    // sample address = %11AAAAAA.AA000000 = $C000 + (A * 64)
    m_sampleAddress = 0xc000 + (getRegisters().sampleAddress * 64);
    // sample length = %LLLL.LLLL0001 = (L * 16) + 1 bytes
    m_sampleLength = (getRegisters().sampleLength * 16) + 1;
  }

  if (!m_isActive)
    return;

  m_dividerCount++;
  if (m_dividerCount >= m_dividerPeriod) {
    m_dividerCount = 0;
  } else {
    return;
  }

  bool needFetch = (m_buffer == -1 || m_cursorBit == 8);
  if (needFetch) {
    int nextByte = m_cursorByte + 1;

    if (nextByte >= m_sampleLength) {
      m_isActive = false;
      m_buffer = -1;
      if (getRegisters().control.loop) {
        start();
      }
      return;
    }

    m_cursorByte = nextByte;
    m_cursorBit = 0;

    u32 address = m_sampleAddress + m_cursorByte;
    if (address > 0xffff) {
      address = 0x8000 + (address % 0x10000);
    }
    m_buffer = getCPU()->memory().read(address);
  }

  bool bitValue = ((m_buffer >> m_cursorBit) & 1) != 0;
  int variation = bitValue ? 2 : -2;
  int newSample = m_dmcChannel->m_outputSample + variation;

  if (newSample >= 0 && newSample <= 127) {
    m_dmcChannel->m_outputSample = static_cast<u8>(newSample);
  }

  m_cursorBit++;
}

void DPCM::start() { m_startFlag = true; }

void DPCM::stop() { m_cursorByte = m_sampleLength; }

u32 DPCM::remainingBytes() const {
  if (!m_isActive)
    return 0;
  return static_cast<u32>(std::max(0, m_sampleLength - (m_cursorByte + 1)));
}
