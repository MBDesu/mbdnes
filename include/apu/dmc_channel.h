#pragma once
#ifndef MODULE_APU_DMC_CHANNEL_H
#define MODULE_APU_DMC_CHANNEL_H

#include "apu/audio_registers.h"
#include "globals.h"

class APU;
class CPU;
class DPCM;

class DMCChannel {
private:
  APU *m_apu;
  CPU *m_cpu;
  DPCM *m_dpcm;

public:
  u8 m_outputSample = 0;

  DMCChannel(APU *apu, CPU *cpu);
  ~DMCChannel();

  void step();
  float sample() const { return static_cast<float>(m_outputSample); }

  u32 getRemainingBytes() const;
  void startDPCM();

  CPU *getCPU() const { return m_cpu; }
  DMCRegisters &getRegisters() const;
  void stopDPCM();
};

#endif // MODULE_APU_DMC_CHANNEL_H
