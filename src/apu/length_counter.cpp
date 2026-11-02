#include "apu/length_counter.h"

void LengthCounter::clock(bool isEnabled, bool isHalted) {
  if (!isEnabled) {
    reset();
  } else if (isActive() && !isHalted) {
    m_counter--;
  }
}

void LinearLengthCounter::fullReset() {
  reset();
  m_reload = 0;
  m_reloadFlag = false;
}

void LinearLengthCounter::clock(bool isEnabled, bool isHalted) {
  if (!isEnabled) {
    reset();
    return;
  }
  if (m_reloadFlag) {
    m_counter = m_reload;
    m_reloadFlag = false;
  } else if (m_counter > 0 && !isHalted) {
    m_counter--;
  }
}
