#pragma once
#ifndef MODULE_APU_FREQUENCY_SWEEP_H
#define MODULE_APU_FREQUENCY_SWEEP_H

#include "globals.h"

class PulseChannel;

class FrequencySweep {
private:
  PulseChannel *m_channel;

public:
  bool m_startFlag = false;
  u8 m_dividerCount = 0;
  bool m_mute = false;

  explicit FrequencySweep(PulseChannel *ch) : m_channel(ch) {}
  void clock();
  void muteIfNeeded();
};

#endif // MODULE_APU_FREQUENCY_SWEEP_H
