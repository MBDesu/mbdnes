#include "apu/frame_sequencer.h"
#include "apu/apu.h"

void FrameSequencer::step() {
  m_counter++;

  bool is5StepMode = m_apu->m_registers.m_apuFrameCounter.use5StepSequencer;
  u16 maxTicks = is5StepMode ? 18641 : 14916;

  if (m_counter == 3729) {
    m_apu->onQuarterFrameClock();
  } else if (m_counter == 7457) {
    m_apu->onQuarterFrameClock();
    m_apu->onHalfFrameClock();
  } else if (m_counter == 11186) {
    m_apu->onQuarterFrameClock();
  } else if (m_counter == 14916 && !is5StepMode) {
    m_apu->onQuarterFrameClock();
    m_apu->onHalfFrameClock();
  } else if (m_counter == 18641 && is5StepMode) {
    m_apu->onQuarterFrameClock();
    m_apu->onHalfFrameClock();
  }

  if (m_counter >= maxTicks) {
    reset();
  }
}
