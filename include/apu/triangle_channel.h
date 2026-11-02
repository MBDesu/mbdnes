#pragma once
#ifndef MODULE_APU_TRIANGLE_CHANNEL_H
#define MODULE_APU_TRIANGLE_CHANNEL_H

#include "apu/length_counter.h"

class APU;
class TriangleOscillator;

class TriangleChannel {
private:
  APU *m_apu;
  u16 m_timerDivider = 0;

public:
  TriangleOscillator *m_oscillator;
  LinearLengthCounter m_linearLengthCounter;
  LengthCounter m_lengthCounter;
  float m_lastSample = 0.0f;

  explicit TriangleChannel(APU *apu);
  ~TriangleChannel();

  float sample();
  void quarterFrame();
  void halfFrame();
  bool isEnabled() const;
};

#endif // MODULE_APU_TRIANGLE_CHANNEL_H
