#pragma once
#ifndef MODULE_APU_TRIANGLE_OSCILLATOR_H
#define MODULE_APU_TRIANGLE_OSCILLATOR_H

#include "apu.h"
#include "globals.h"
#include <cmath>

constexpr float APU_SAMPLE_RATE = 44100.0f;
constexpr u8 TRIANGLE_SEQUENCE[32] = {15, 14, 13, 12, 11, 10, 9,  8,  7,  6, 5,
                                      4,  3,  2,  1,  0,  0,  1,  2,  3,  4, 5,
                                      6,  7,  8,  9,  10, 11, 12, 13, 14, 15};
constexpr float INVERSE_SAMPLE_RATE = 1.0f / 44100.0f;
class TriangleOscillator {
private:
public:
  float m_frequency = 0.0f;
  float m_phase = 0.0f;
  APU *m_apu;

  TriangleOscillator(APU *apu) : m_apu(apu) {};

  u8 sample() {
    m_phase += m_frequency * INVERSE_SAMPLE_RATE;
    while (m_phase >= 1.0f)
      m_phase -= 1.0f;
    while (m_phase < 0.0f)
      m_phase += 1.0f;
    auto step = static_cast<u8>(std::floor(m_phase * 32.0f));

    return TRIANGLE_SEQUENCE[step];
  }
};

#endif // MODULE_APU_TRIANGLE_OSCILLATOR_H
