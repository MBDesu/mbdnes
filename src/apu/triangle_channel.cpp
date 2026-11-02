
#include "apu/triangle_channel.h"
#include "apu/apu.h"
#include "apu/triangle_oscillator.h"

TriangleChannel::TriangleChannel(APU *apu) : m_apu(apu) {
  m_oscillator = new TriangleOscillator(apu);
}

TriangleChannel::~TriangleChannel() { delete m_oscillator; }

bool TriangleChannel::isEnabled() const {
  return m_apu->m_registers.m_apuControl.enableTriangle;
}

float TriangleChannel::sample() {
  if (!isEnabled() || !m_lengthCounter.isActive() ||
      !m_linearLengthCounter.isActive())
    return m_lastSample;
  const u16 timer = (m_apu->m_registers.m_triangle.timerHighLCL.timerHigh)
                        << 8 |
                    m_apu->m_registers.m_triangle.timerLow;
  if (timer < 2 || timer > 0x7ff)
    return 0;
  m_oscillator->m_frequency = 1789773.0 / (16.0 * (timer + 1.0)) / 2.0;
  m_lastSample = m_oscillator->sample();
  return m_lastSample;
}

void TriangleChannel::quarterFrame() {
  bool isHalted = m_apu->m_registers.m_triangle.lengthControl.halt;
  m_linearLengthCounter.clock(isEnabled(), isHalted);
}

void TriangleChannel::halfFrame() {
  bool isHalted = m_apu->m_registers.m_triangle.lengthControl.halt;
  m_lengthCounter.clock(isEnabled(), isHalted);
}
