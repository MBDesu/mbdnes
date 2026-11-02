#pragma once
#ifndef MODULE_APU_VOLUME_ENVELOPE_H
#define MODULE_APU_VOLUME_ENVELOPE_H

#include "globals.h"

class VolumeEnvelope {
public:
  bool m_startFlag = false;
  u8 m_dividerCount = 0;
  u8 m_volume = 0;

  VolumeEnvelope() = default;
  void clock(u8 period, bool loop);
};

#endif // MODULE_APU_VOLUME_ENVELOPE_H
