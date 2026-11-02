#pragma once
#ifndef MODULE_APU_AUDIO_REGISTERS_H
#define MODULE_APU_AUDIO_REGISTERS_H

#include "globals.h"
#include "lib/byte.h"

class APU;

class AudioRegister {
public:
  u8 raw;

  AudioRegister() : raw(0) {}
  AudioRegister(u8 value) : raw(value) {}

  AudioRegister &operator=(u8 value) {
    raw = value;
    return *this;
  }
};

struct PulseControlRegister : public AudioRegister {
  using AudioRegister::AudioRegister;

  PulseControlRegister() : AudioRegister(0) {}
  PulseControlRegister(const PulseControlRegister &other)
      : AudioRegister(other.raw) {}
  PulseControlRegister &operator=(const PulseControlRegister &other) {
    this->raw = other.raw;
    return *this;
  }

  Bits<u8, 0, 4> volumeOrEnvelopePeriod{raw};
  Bit<u8, 4> constantVolume{raw};
  Bit<u8, 5> envelopeLoopOrLengthCounterHalt{raw};
  Bits<u8, 6, 2> dutyCycleId{raw};
};

struct PulseSweepRegister : public AudioRegister {
  using AudioRegister::AudioRegister;

  PulseSweepRegister() : AudioRegister(0) {}
  PulseSweepRegister(const PulseSweepRegister &other)
      : AudioRegister(other.raw) {}
  PulseSweepRegister &operator=(const PulseSweepRegister &other) {
    this->raw = other.raw;
    return *this;
  }

  Bits<u8, 0, 3> shiftCount{raw};
  Bit<u8, 3> negateFlag{raw};
  Bits<u8, 4, 3> dividerPeriodMinusOne{raw};
  Bit<u8, 7> enabledFlag{raw};
};

struct PulseTimerHighLCLRegister : public AudioRegister {
  using AudioRegister::AudioRegister;

  PulseTimerHighLCLRegister() : AudioRegister(0) {}
  PulseTimerHighLCLRegister(const PulseTimerHighLCLRegister &other)
      : AudioRegister(other.raw) {}
  PulseTimerHighLCLRegister &operator=(const PulseTimerHighLCLRegister &other) {
    this->raw = other.raw;
    return *this;
  }

  Bits<u8, 0, 3> timerHigh{raw};
  Bits<u8, 3, 5> lengthCounterLoad{raw};
};

struct TriangleLengthControlRegister : public AudioRegister {
  using AudioRegister::AudioRegister;

  TriangleLengthControlRegister() : AudioRegister(0) {}
  TriangleLengthControlRegister(const TriangleLengthControlRegister &other)
      : AudioRegister(other.raw) {}
  TriangleLengthControlRegister &
  operator=(const TriangleLengthControlRegister &other) {
    this->raw = other.raw;
    return *this;
  }

  Bits<u8, 0, 7> linearCounterReload{raw};
  Bit<u8, 7> halt{raw};
};

struct TriangleTimerHighLCLRegister : public AudioRegister {
  using AudioRegister::AudioRegister;

  TriangleTimerHighLCLRegister() : AudioRegister(0) {}
  TriangleTimerHighLCLRegister(const TriangleTimerHighLCLRegister &other)
      : AudioRegister(other.raw) {}
  TriangleTimerHighLCLRegister &
  operator=(const TriangleTimerHighLCLRegister &other) {
    this->raw = other.raw;
    return *this;
  }

  Bits<u8, 0, 3> timerHigh{raw};
  Bits<u8, 3, 5> lengthCounterLoad{raw};
};

struct NoiseControlRegister : public AudioRegister {
  using AudioRegister::AudioRegister;

  NoiseControlRegister() : AudioRegister(0) {}
  NoiseControlRegister(const NoiseControlRegister &other)
      : AudioRegister(other.raw) {}
  NoiseControlRegister &operator=(const NoiseControlRegister &other) {
    this->raw = other.raw;
    return *this;
  }

  Bits<u8, 0, 4> volumeOrEnvelopePeriod{raw};
  Bit<u8, 4> constantVolume{raw};
  Bit<u8, 5> envelopeLoopOrLengthCounterHalt{raw};
};

struct NoiseFormRegister : public AudioRegister {
  using AudioRegister::AudioRegister;

  NoiseFormRegister() : AudioRegister(0) {}
  NoiseFormRegister(const NoiseFormRegister &other)
      : AudioRegister(other.raw) {}
  NoiseFormRegister &operator=(const NoiseFormRegister &other) {
    this->raw = other.raw;
    return *this;
  }

  Bits<u8, 0, 4> periodId{raw};
  Bit<u8, 7> mode{raw};
};

struct NoiseLCLRegister : public AudioRegister {
  using AudioRegister::AudioRegister;

  NoiseLCLRegister() : AudioRegister(0) {}
  NoiseLCLRegister(const NoiseLCLRegister &other) : AudioRegister(other.raw) {}
  NoiseLCLRegister &operator=(const NoiseLCLRegister &other) {
    this->raw = other.raw;
    return *this;
  }

  Bits<u8, 3, 5> lengthCounterLoad{raw};
};

struct DMCControlRegister : public AudioRegister {
  using AudioRegister::AudioRegister;

  DMCControlRegister() : AudioRegister(0) {}
  DMCControlRegister(const DMCControlRegister &other)
      : AudioRegister(other.raw) {}
  DMCControlRegister &operator=(const DMCControlRegister &other) {
    this->raw = other.raw;
    return *this;
  }

  Bits<u8, 0, 4> dpcmPeriodId{raw};
  Bit<u8, 6> loop{raw};
};

struct DMCLoadRegister : public AudioRegister {
  using AudioRegister::AudioRegister;

  DMCLoadRegister() : AudioRegister(0) {}
  DMCLoadRegister(const DMCLoadRegister &other) : AudioRegister(other.raw) {}
  DMCLoadRegister &operator=(const DMCLoadRegister &other) {
    this->raw = other.raw;
    return *this;
  }

  Bits<u8, 0, 7> directLoad{raw};
};

struct APUControlRegister : public AudioRegister {
  using AudioRegister::AudioRegister;

  APUControlRegister() : AudioRegister(0) {}
  APUControlRegister(const APUControlRegister &other)
      : AudioRegister(other.raw) {}
  APUControlRegister &operator=(const APUControlRegister &other) {
    this->raw = other.raw;
    return *this;
  }

  Bit<u8, 0> enablePulse1{raw};
  Bit<u8, 1> enablePulse2{raw};
  Bit<u8, 2> enableTriangle{raw};
  Bit<u8, 3> enableNoise{raw};
  Bit<u8, 4> enableDMC{raw};
};

struct APUFrameCounterRegister : public AudioRegister {
  using AudioRegister::AudioRegister;

  APUFrameCounterRegister() : AudioRegister(0) {}
  APUFrameCounterRegister(const APUFrameCounterRegister &other)
      : AudioRegister(other.raw) {}
  APUFrameCounterRegister &operator=(const APUFrameCounterRegister &other) {
    this->raw = other.raw;
    return *this;
  }

  Bit<u8, 7> use5StepSequencer{raw};
};

struct PulseRegisters {
  PulseControlRegister control;
  PulseSweepRegister sweep;
  u8 timerLow = 0;
  PulseTimerHighLCLRegister timerHighLCL;
};

struct TriangleRegisters {
  TriangleLengthControlRegister lengthControl;
  u8 timerLow = 0;
  TriangleTimerHighLCLRegister timerHighLCL;
};

struct NoiseRegisters {
  NoiseControlRegister control;
  NoiseFormRegister form;
  NoiseLCLRegister lcl;
};

struct DMCRegisters {
  DMCControlRegister control;
  DMCLoadRegister load;
  u8 sampleAddress = 0;
  u8 sampleLength = 0;
};

class AudioRegisters {
private:
  APU *m_apu;
  AudioRegister &getRegister(u16 address);

public:
  PulseRegisters m_pulses[2];
  TriangleRegisters m_triangle;
  NoiseRegisters m_noise;
  DMCRegisters m_dmc;

  APUControlRegister m_apuControl;
  APUFrameCounterRegister m_apuFrameCounter;

  explicit AudioRegisters(APU *apu);

  u8 read(u16 address);
  void write(u16 address, u8 value);
};

#endif // MODULE_APU_AUDIO_REGISTERS_H
