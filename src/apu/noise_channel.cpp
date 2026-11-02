#include "apu/noise_channel.h"
#include "apu/apu.h"
#include "apu/noise_periods.h"

NoiseChannel::NoiseChannel(APU *apu) : m_apu(apu) {}

bool NoiseChannel::isEnabled() const {
  return m_apu->m_registers.m_apuControl.enableNoise;
}

u8 NoiseChannel::sample() {
  if (!isEnabled() || !m_lengthCounter.isActive() || (m_shift & 1)) {
    return 0;
  }
  auto &reg = m_apu->m_registers.m_noise;
  u8 volume = reg.control.constantVolume ? reg.control.volumeOrEnvelopePeriod
                                         : m_volumeEnvelope.m_volume;
  return volume;
}

void NoiseChannel::step() {
  auto &reg = m_apu->m_registers.m_noise;
  m_dividerCount++;
  if (m_dividerCount >= noisePeriods[reg.form.periodId]) {
    m_dividerCount = 0;
  } else {
    return;
  }
  u8 bit = reg.form.mode ? 6 : 1;
  u16 feedback = (m_shift & 1) ^ ((m_shift >> bit) & 1);
  m_shift >>= 1;
  m_shift = (m_shift & 0x3fff) | (feedback << 14);
}

void NoiseChannel::quarterFrame() {
  auto &reg = m_apu->m_registers.m_noise;
  m_volumeEnvelope.clock(reg.control.volumeOrEnvelopePeriod,
                         reg.control.envelopeLoopOrLengthCounterHalt);
}

void NoiseChannel::halfFrame() {
  auto &reg = m_apu->m_registers.m_noise;
  m_lengthCounter.clock(isEnabled(),
                        reg.control.envelopeLoopOrLengthCounterHalt);
}

double NoiseChannel::random() {
  m_s = (m_s * 1664525 + 1013904223);
  return static_cast<double>(m_s) / 4294967296.0;
}
