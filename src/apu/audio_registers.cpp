#include "apu/audio_registers.h"
#include "apu/apu.h"
#include "apu/note_lengths.h"

AudioRegisters::AudioRegisters(APU *apu) : m_apu(apu) {}

u8 AudioRegisters::read(u16 address) {
  if (address == 0x4015) {
    u8 bit0 = m_apu->m_pulse1.m_lengthCounter.isActive() ? 1 : 0;
    u8 bit1 = m_apu->m_pulse2.m_lengthCounter.isActive() ? 1 : 0;
    u8 bit2 = m_apu->m_triangle.m_lengthCounter.isActive() ? 1 : 0;
    u8 bit3 = m_apu->m_noise.m_lengthCounter.isActive() ? 1 : 0;
    u8 bit4 = (m_apu->m_dmc.getRemainingBytes() > 0) ? 1 : 0;
    return bit0 | (bit1 << 1) | (bit2 << 2) | (bit3 << 3) | (bit4 << 4);
  }
  return 0;
}

void AudioRegisters::write(u16 address, u8 value) {
  switch (address) {
  // Pulse 1
  case 0x4000:
    m_pulses[0].control = value;
    break;
  case 0x4001:
    m_pulses[0].sweep = value;
    m_apu->m_pulse1.m_frequencySweep.m_startFlag = true;
    break;
  case 0x4002:
    m_pulses[0].timerLow = value;
    m_apu->m_pulse1.updateTimer();
    break;
  case 0x4003:
    m_pulses[0].timerHighLCL = value;
    m_apu->m_pulse1.updateTimer();
    if (m_apuControl.enablePulse1) {
      m_apu->m_pulse1.m_lengthCounter.m_counter =
          noteLengths[m_pulses[0].timerHighLCL.lengthCounterLoad];
    }
    m_apu->m_pulse1.m_volumeEnvelope.m_startFlag = true;
    break;

  // Pulse 2
  case 0x4004:
    m_pulses[1].control = value;
    break;
  case 0x4005:
    m_pulses[1].sweep = value;
    m_apu->m_pulse2.m_frequencySweep.m_startFlag = true;
    break;
  case 0x4006:
    m_pulses[1].timerLow = value;
    m_apu->m_pulse2.updateTimer();
    break;
  case 0x4007:
    m_pulses[1].timerHighLCL = value;
    m_apu->m_pulse2.updateTimer();
    if (m_apuControl.enablePulse2) {
      m_apu->m_pulse2.m_lengthCounter.m_counter =
          noteLengths[m_pulses[1].timerHighLCL.lengthCounterLoad];
    }
    m_apu->m_pulse2.m_volumeEnvelope.m_startFlag = true;
    break;

  // Triangle
  case 0x4008: {
    m_triangle.lengthControl = value;
    m_apu->m_triangle.m_linearLengthCounter.m_reload =
        m_triangle.lengthControl.linearCounterReload;
    // m_apu->m_triangle.m_linearLengthCounter.m_reloadFlag = true;
    break;
  }
  case 0x400A:
    m_triangle.timerLow = value;
    break;
  case 0x400B:
    m_triangle.timerHighLCL = value;
    if (m_apuControl.enableTriangle) {
      m_apu->m_triangle.m_lengthCounter.m_counter =
          noteLengths[m_triangle.timerHighLCL.lengthCounterLoad];
    }
    m_apu->m_triangle.m_linearLengthCounter.m_reloadFlag = true;
    break;

  // Noise
  case 0x400C:
    m_noise.control = value;
    break;
  case 0x400E:
    m_noise.form = value;
    break;
  case 0x400F:
    m_noise.lcl = value;
    if (m_apuControl.enableNoise) {
      m_apu->m_noise.m_lengthCounter.m_counter =
          noteLengths[m_noise.lcl.lengthCounterLoad];
    }
    m_apu->m_noise.m_volumeEnvelope.m_startFlag = true;
    break;

  // DMC
  case 0x4010:
    m_dmc.control = value;
    break;
  case 0x4011:
    m_dmc.load = value;
    m_apu->m_dmc.m_outputSample = m_dmc.load.directLoad;
    break;
  case 0x4012:
    m_dmc.sampleAddress = value;
    break;
  case 0x4013:
    m_dmc.sampleLength = value;
    break;

  // Controls
  case 0x4015: {
    m_apuControl = value;
    if (!m_apuControl.enablePulse1)
      m_apu->m_pulse1.m_lengthCounter.reset();
    if (!m_apuControl.enablePulse2)
      m_apu->m_pulse2.m_lengthCounter.reset();
    if (!m_apuControl.enableTriangle) {
      m_apu->m_triangle.m_lengthCounter.reset();
      m_apu->m_triangle.m_linearLengthCounter.fullReset();
    }
    if (!m_apuControl.enableNoise)
      m_apu->m_noise.m_lengthCounter.reset();
    if (!m_apuControl.enableDMC) {
      m_apu->m_dmc.stopDPCM();
    } else {
      if (m_apu->m_dmc.getRemainingBytes() == 0) {
        m_apu->m_dmc.startDPCM();
      }
    }
    break;
  }
  case 0x4017:
    m_apuFrameCounter = value;

    m_apu->m_frameSequencer.reset();
    m_apu->onQuarterFrameClock();
    m_apu->onHalfFrameClock();
    break;

  default:
    break;
  }
}
