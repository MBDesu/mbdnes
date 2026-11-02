#pragma once
#ifndef MODULE_APU_PULSE_OSCILLATOR_H
#define MODULE_APU_PULSE_OSCILLATOR_H

#include "globals.h"

class PulseOscillator {
private:
  const float m_apuSampleRate = 44100.0f;
  const float m_dutyTable[4] = {0.125f, 0.25f, 0.5f, 0.75f};

public:
  float m_frequency = 0.0f;
  u8 m_dutyCycle = 0;
  u8 m_volume = 15;
  float m_phase = 0.0f;

  PulseOscillator() = default;

  u8 sample() {
    m_phase = m_phase + (m_frequency / m_apuSampleRate);
    if (m_phase >= 1.0f) {
      m_phase -= 1.0f;
    }

    return (m_phase < m_dutyTable[m_dutyCycle]) ? m_volume : 0;
  }
};

#endif // MODULE_APU_PULSE_OSCILLATOR_H
