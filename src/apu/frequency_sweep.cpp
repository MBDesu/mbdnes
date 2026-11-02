#include "apu/frequency_sweep.h"
#include "apu/audio_registers.h"
#include "apu/pulse_channel.h"

void FrequencySweep::clock() {
  auto &registerSweep = m_channel->getRegisters().sweep;

  muteIfNeeded();

  if (registerSweep.enabledFlag && registerSweep.shiftCount > 0 &&
      m_dividerCount == 0 && !m_mute) {
    u16 sweepDelta = m_channel->m_timer >> registerSweep.shiftCount;
    if (registerSweep.negateFlag) {
      if (m_channel->m_id == 0) {
        m_channel->m_timer -= (sweepDelta + 1);
      } else {
        m_channel->m_timer -= sweepDelta;
      }
    } else {
      m_channel->m_timer += sweepDelta;
    }
  }
  if (m_dividerCount == 0 || m_startFlag) {
    m_dividerCount = registerSweep.dividerPeriodMinusOne;
    m_startFlag = false;
  } else {
    m_dividerCount--;
  }
}

void FrequencySweep::muteIfNeeded() {
  auto &registerSweep = m_channel->getRegisters().sweep;
  u16 currentTimer = m_channel->m_timer;

  u16 sweepDelta = currentTimer >> registerSweep.shiftCount;
  u16 targetTimer = currentTimer;

  if (registerSweep.negateFlag) {
    if (m_channel->m_id == 0) {
      targetTimer = (currentTimer >= sweepDelta + 1)
                        ? currentTimer - (sweepDelta + 1)
                        : 0;
    } else {
      targetTimer =
          (currentTimer >= sweepDelta) ? currentTimer - sweepDelta : 0;
    }
  } else {
    targetTimer = currentTimer + sweepDelta;
  }
  m_mute = (m_channel->m_timer < 8 || m_channel->m_timer > 0x7ff);
}
