#pragma once
#ifndef MODULE_PLATFORM_CONTROLLER_H
#define MODULE_PLATFORM_CONTROLLER_H

#include "cpu/cpu_memory.h"
#include "globals.h"
#include <SDL2/SDL.h>

enum NESButtons : u8 {
  A = (1 << 0),
  B = (1 << 1),
  Select = (1 << 2),
  Start = (1 << 3),
  Up = (1 << 4),
  Down = (1 << 5),
  Left = (1 << 6),
  Right = (1 << 7),
};

struct ControllerState {
  void *p1 = nullptr; // SDL_GameController*
  void *p2 = nullptr;
};

class InputManager {
public:
  InputManager();
  virtual ~InputManager();
  void handleEvent(const SDL_Event &event);
  void pollInputs(class CPUMemory &memory);

private:
  ControllerState *state;
};

#endif // MODULE_PLATFORM_CONTROLLER_H
