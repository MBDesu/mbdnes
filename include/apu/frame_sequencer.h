#pragma once
#ifndef MODULE_APU_FRAME_SEQUENCER_H
#define MODULE_APU_FRAME_SEQUENCER_H

#include "globals.h"

class APU;

class FrameSequencer {
private:
  APU *m_apu;
  const u16 m_fourStep[4] = {3729, 7457, 11186, 14916};
  const u16 m_fiveStep[4] = {3729, 7457, 11186, 18641};

public:
  u32 m_counter = 0;

  explicit FrameSequencer(APU *apu) : m_apu(apu) { reset(); }
  void reset() { m_counter = 0; }
  void step();
};

#endif // MODULE_APU_FRAME_SEQUENCER_H
