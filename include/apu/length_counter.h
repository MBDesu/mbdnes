#pragma once
#ifndef MODULE_APU_LENGTH_COUNTER_H
#define MODULE_APU_LENGTH_COUNTER_H

#include "globals.h"

class LengthCounter {
public:
  u8 m_counter = 0;

  LengthCounter() = default;
  virtual ~LengthCounter() = default;

  void reset() { m_counter = 0; }
  bool isActive() const { return m_counter > 0; }
  virtual void clock(bool isEnabled, bool isHalted);
};

class LinearLengthCounter : public LengthCounter {
public:
  u8 m_reload = 0;
  bool m_reloadFlag = false;

  LinearLengthCounter() = default;

  void fullReset();
  void clock(bool isEnabled, bool isHalted) override;
};

#endif // MODULE_APU_LENGTH_COUNTER_H
