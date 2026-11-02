#include "apu/volume_envelope.h"

void VolumeEnvelope::clock(u8 period, bool loop) {
  if (m_startFlag) {
    m_startFlag = false;
    m_volume = 15;
    m_dividerCount = period;
    return;
  }
  if (m_dividerCount > 0) {
    m_dividerCount--;
    return;
  }
  m_dividerCount = period;
  if (m_volume == 0) {
    if (loop) {
      m_volume = 15;
    }
  } else {
    m_volume--;
  }
}
