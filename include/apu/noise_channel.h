#pragma once
#ifndef MODULE_APU_NOISE_CHANNEL_H
#define MODULE_APU_NOISE_CHANNEL_H

#include "apu/length_counter.h"
#include "apu/volume_envelope.h"
#include "globals.h"

class APU;

class NoiseChannel {
private:
  APU *m_apu;
  u32 m_s = 0x9e3779b9;

public:
  LengthCounter m_lengthCounter;
  VolumeEnvelope m_volumeEnvelope;
  u16 m_shift = 1;
  u16 m_dividerCount = 0;

  explicit NoiseChannel(APU *apuRef);

  u8 sample();
  void step();
  void quarterFrame();
  void halfFrame();
  bool isEnabled() const;
  double random();
};

#endif // MODULE_APU_NOISE_CHANNEL_H
