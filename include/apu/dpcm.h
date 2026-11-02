#pragma once
#ifndef MODULE_APU_DPCM_H
#define MODULE_APU_DPCM_H

#include "globals.h"

class DMCChannel;
class CPU;
struct DMCRegisters;

class DPCM {
private:
  DMCChannel *m_dmcChannel;

  CPU *getCPU() const;
  DMCRegisters &getRegisters() const;

public:
  bool m_startFlag = false;
  bool m_isActive = false;

  int m_buffer = -1;

  int m_cursorByte = 0;
  u8 m_cursorBit = 0;
  u16 m_dividerPeriod = 0;
  u16 m_dividerCount = 0;
  u16 m_sampleAddress = 0;
  int m_sampleLength = 0;

  explicit DPCM(DMCChannel *dmcChannel);

  void update();
  void start();
  void stop();
  u32 remainingBytes() const;
};

#endif // MODULE_APU_DPCM_H
