#pragma once
#ifndef MODULE_APU_PULSE_CHANNEL_H
#define MODULE_APU_PULSE_CHANNEL_H

#include "apu/frequency_sweep.h"
#include "apu/length_counter.h"
#include "apu/volume_envelope.h"
#include "globals.h"

class APU;
struct PulseRegisters;
class PulseOscillator;

class PulseChannel {
private:
  APU *m_apu;
  PulseRegisters *m_registers;

public:
  u16 m_timer = 0;
  u8 m_id;
  PulseOscillator *m_oscillator;
  LengthCounter m_lengthCounter;
  VolumeEnvelope m_volumeEnvelope;
  FrequencySweep m_frequencySweep;
  float m_lastSample = 0.0f;

  PulseChannel(APU *apu, u8 channelId, PulseRegisters *pulseRegisters);
  ~PulseChannel();

  float sample();
  void updateTimer();
  void step();
  bool isEnabled() const;
  void quarterFrame();
  void halfFrame();

  PulseRegisters &getRegisters() const;
};

#endif // MODULE_APU_PULSE_CHANNEL_H
