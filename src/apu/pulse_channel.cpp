#include "apu/pulse_channel.h"
#include "apu/apu.h"
#include "apu/pulse_oscillator.h"

PulseChannel::PulseChannel(APU *apu, u8 channelId,
                           PulseRegisters *pulseRegisters)
    : m_apu(apu), m_id(channelId), m_frequencySweep(this),
      m_registers(pulseRegisters) {
  m_oscillator = new PulseOscillator();
}

PulseChannel::~PulseChannel() { delete m_oscillator; }

PulseRegisters &PulseChannel::getRegisters() const { return *m_registers; }

bool PulseChannel::isEnabled() const {
  if (m_id == 0)
    return m_apu->m_registers.m_apuControl.enablePulse1;
  return m_apu->m_registers.m_apuControl.enablePulse2;
}

void PulseChannel::updateTimer() {
  auto &reg = getRegisters();
  m_timer = (static_cast<u16>(reg.timerHighLCL.timerHigh) << 8) | reg.timerLow;
}

void PulseChannel::step() {
  m_frequencySweep.muteIfNeeded();
  if (!getRegisters().sweep.enabledFlag) {
    updateTimer();
  }
}

float PulseChannel::sample() {
  if (m_frequencySweep.m_mute || !isEnabled() || !m_lengthCounter.isActive()) {
    return m_lastSample;
  }
  auto &reg = getRegisters();
  m_oscillator->m_frequency = 1789773.0f / (16.0f * (m_timer + 1));
  m_oscillator->m_dutyCycle = reg.control.dutyCycleId;
  m_oscillator->m_volume = reg.control.constantVolume
                               ? reg.control.volumeOrEnvelopePeriod
                               : m_volumeEnvelope.m_volume;
  m_lastSample = m_oscillator->sample();
  return m_lastSample;
}

void PulseChannel::quarterFrame() {
  auto &reg = getRegisters();
  m_volumeEnvelope.clock(reg.control.volumeOrEnvelopePeriod,
                         reg.control.envelopeLoopOrLengthCounterHalt);
}

void PulseChannel::halfFrame() {
  auto &reg = getRegisters();
  m_lengthCounter.clock(isEnabled(),
                        reg.control.envelopeLoopOrLengthCounterHalt);
  m_frequencySweep.clock();
}
