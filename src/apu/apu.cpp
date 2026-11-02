#include "apu/apu.h"
#include "cpu/cpu.h"
#include "platform/platform.h"

APU::APU(CPU *cpuRef)
    : m_cpu(cpuRef), m_sampleCounter(0), m_currentSample(0.0f), m_time(0.0),
      m_pulse1(this, 0, &m_registers.m_pulses[0]),
      m_pulse2(this, 1, &m_registers.m_pulses[1]), m_triangle(this),
      m_noise(this), m_dmc(this, cpuRef), m_frameSequencer(this),
      m_registers(this) {}

void APU::onQuarterFrameClock() {
  m_pulse1.quarterFrame();
  m_pulse2.quarterFrame();
  m_triangle.quarterFrame();
  m_noise.quarterFrame();
}

void APU::onHalfFrameClock() {
  m_pulse1.halfFrame();
  m_pulse2.halfFrame();
  m_triangle.halfFrame();
  m_noise.halfFrame();
}

void APU::step(const std::function<void(float, float, float, float, float,
                                        float)> &onSample,
               AudioOverrides *overrides) {
  static bool apuCycleToggle = false;
  apuCycleToggle = !apuCycleToggle;
  if (!apuCycleToggle)
    return;
  m_pulse1.step();
  m_pulse2.step();
  m_noise.step();
  m_dmc.step();
  m_frameSequencer.step();

  m_sampleCounter++;

  if (m_sampleCounter == 20) {
    m_sampleCounter = 0;
    m_time += (1.0 / 44100.0);

    float p1 = m_pulse1.sample();
    float p2 = m_pulse2.sample();
    float tri = m_triangle.sample();
    float n = static_cast<float>(m_noise.sample());
    float d = m_dmc.sample();
    if (overrides != nullptr) {
      if (*(overrides->mutePulse1))
        p1 = 0.0f;
      if (*(overrides->mutePulse2))
        p2 = 0.0f;
      if (*(overrides->muteTriangle))
        tri = 0.0f;
      if (*(overrides->muteNoise))
        n = 0.0f;
      if (*(overrides->muteDmc))
        d = 0.0f;
    }

    m_currentSample = (0.00752f * (p1 + p2)) + (0.00851f * tri) +
                      (0.00494f * n) + (0.00335f * d);

    m_frameAudio.append(m_currentSample, p1, p2, tri, n, d);
  }
}

FrameAudioCapture &APU::getFrameAudio() { return m_frameAudio; }
