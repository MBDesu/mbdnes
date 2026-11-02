#pragma once
#ifndef MODULE_APU_APU_H
#define MODULE_APU_APU_H

#include "apu/audio_registers.h"
#include "apu/dmc_channel.h"
#include "apu/frame_sequencer.h"
#include "apu/noise_channel.h"
#include "apu/pulse_channel.h"
#include "apu/triangle_channel.h"
#include "globals.h"
#include "platform/platform.h"
#include <functional>

class CPU;

struct AudioOverrides {
  bool *mutePulse1;
  bool *mutePulse2;
  bool *muteTriangle;
  bool *muteNoise;
  bool *muteDmc;
};

class APU {
public:
  AudioRegisters m_registers;
  CPU *m_cpu;

  PulseChannel m_pulse1;
  PulseChannel m_pulse2;
  TriangleChannel m_triangle;
  NoiseChannel m_noise;
  DMCChannel m_dmc;

  FrameSequencer m_frameSequencer;
  u16 m_sampleCounter = 0;
  float m_currentSample = 0.0f;
  double m_time = 0.0;

  explicit APU(CPU *cpu);

  FrameAudioCapture &getFrameAudio();
  void onQuarterFrameClock();
  void onHalfFrameClock();
  void step(const std::function<void(float, float, float, float, float, float)>
                &onSample,
            AudioOverrides *overrides = nullptr);

private:
  FrameAudioCapture m_frameAudio;
};

#endif // MODULE_APU_APU_H
