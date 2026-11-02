#include "platform/controller.h"
#include "cpu/cpu_memory.h"
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_log.h>

InputManager::InputManager() : state(nullptr) {}

InputManager::~InputManager() {
  if (state != nullptr) {
    if (state->p1 != nullptr)
      SDL_GameControllerClose(static_cast<SDL_GameController *>(state->p1));
    if (state->p2 != nullptr)
      SDL_GameControllerClose(static_cast<SDL_GameController *>(state->p2));
    delete state;
    state = nullptr;
  }
}

void InputManager::handleEvent(const SDL_Event &event) {
  if (state == nullptr)
    state = new ControllerState();

  switch (event.type) {
  case SDL_CONTROLLERDEVICEADDED: {
    if (state->p1 == nullptr) {
      state->p1 = SDL_GameControllerOpen(event.cdevice.which);
      SDL_LogInfo(
          SDL_LOG_CATEGORY_SYSTEM, "P1 controller connected: %s",
          SDL_GameControllerName(static_cast<SDL_GameController *>(state->p1)));
    } else if (state->p2 == nullptr) {
      state->p2 = SDL_GameControllerOpen(event.cdevice.which);
      SDL_Log(
          "Player 2 Gamepad Connected: %s",
          SDL_GameControllerName(static_cast<SDL_GameController *>(state->p2)));
    }
    break;
  }
  case SDL_CONTROLLERDEVICEREMOVED: {
    if (state->p1 != nullptr) {
      auto *controller = static_cast<SDL_GameController *>(state->p1);
      if (event.cdevice.which ==
          SDL_JoystickInstanceID(SDL_GameControllerGetJoystick(controller))) {
        SDL_GameControllerClose(controller);
        state->p1 = nullptr;
        SDL_LogInfo(SDL_LOG_CATEGORY_SYSTEM, "P1 controller disconnected");
      }
    }
    if (state->p2 != nullptr) {
      auto *controller = static_cast<SDL_GameController *>(state->p2);
      if (event.cdevice.which ==
          SDL_JoystickInstanceID(SDL_GameControllerGetJoystick(controller))) {
        SDL_GameControllerClose(controller);
        state->p2 = nullptr;
        SDL_LogInfo(SDL_LOG_CATEGORY_SYSTEM, "P2 controller disconnected");
      }
    }
    break;
  }
  }
}

void InputManager::pollInputs(CPUMemory &memory) {
  if (state == nullptr)
    state = new ControllerState();
  u8 p1Mask = 0;

  // keyboard
  const u8 *keyState = SDL_GetKeyboardState(NULL);
  if (keyState[SDL_SCANCODE_K])
    p1Mask |= NESButtons::A;
  if (keyState[SDL_SCANCODE_J])
    p1Mask |= NESButtons::B;
  if (keyState[SDL_SCANCODE_SPACE])
    p1Mask |= NESButtons::Select;
  if (keyState[SDL_SCANCODE_RETURN])
    p1Mask |= NESButtons::Start;
  if (keyState[SDL_SCANCODE_W])
    p1Mask |= NESButtons::Up;
  if (keyState[SDL_SCANCODE_S])
    p1Mask |= NESButtons::Down;
  if (keyState[SDL_SCANCODE_A])
    p1Mask |= NESButtons::Left;
  if (keyState[SDL_SCANCODE_D])
    p1Mask |= NESButtons::Right;

  // controller 1
  if (state->p1 != nullptr) {
    auto *controller = static_cast<SDL_GameController *>(state->p1);

    if (SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_B))
      p1Mask |= NESButtons::A;
    if (SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_A))
      p1Mask |= NESButtons::B;
    if (SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_BACK))
      p1Mask |= NESButtons::Select;
    if (SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_START))
      p1Mask |= NESButtons::Start;
    if (SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_DPAD_UP))
      p1Mask |= NESButtons::Up;
    if (SDL_GameControllerGetButton(controller,
                                    SDL_CONTROLLER_BUTTON_DPAD_DOWN))
      p1Mask |= NESButtons::Down;
    if (SDL_GameControllerGetButton(controller,
                                    SDL_CONTROLLER_BUTTON_DPAD_LEFT))
      p1Mask |= NESButtons::Left;
    if (SDL_GameControllerGetButton(controller,
                                    SDL_CONTROLLER_BUTTON_DPAD_RIGHT))
      p1Mask |= NESButtons::Right;

    int16_t axisX =
        SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_LEFTX);
    int16_t axisY =
        SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_LEFTY);
    constexpr int16_t DEADZONE = 12000;
    if (axisX < -DEADZONE)
      p1Mask |= NESButtons::Left;
    if (axisX > DEADZONE)
      p1Mask |= NESButtons::Right;
    if (axisY < -DEADZONE)
      p1Mask |= NESButtons::Up;
    if (axisY > DEADZONE)
      p1Mask |= NESButtons::Down;
  }

  u8 p2Mask = 0;
  if (state->p2 != nullptr) {
    auto *pad = static_cast<SDL_GameController *>(state->p2);

    if (SDL_GameControllerGetButton(pad, SDL_CONTROLLER_BUTTON_B))
      p2Mask |= NESButtons::A;
    if (SDL_GameControllerGetButton(pad, SDL_CONTROLLER_BUTTON_A))
      p2Mask |= NESButtons::B;
    if (SDL_GameControllerGetButton(pad, SDL_CONTROLLER_BUTTON_BACK))
      p2Mask |= NESButtons::Select;
    if (SDL_GameControllerGetButton(pad, SDL_CONTROLLER_BUTTON_START))
      p2Mask |= NESButtons::Start;
    if (SDL_GameControllerGetButton(pad, SDL_CONTROLLER_BUTTON_DPAD_UP))
      p2Mask |= NESButtons::Up;
    if (SDL_GameControllerGetButton(pad, SDL_CONTROLLER_BUTTON_DPAD_DOWN))
      p2Mask |= NESButtons::Down;
    if (SDL_GameControllerGetButton(pad, SDL_CONTROLLER_BUTTON_DPAD_LEFT))
      p2Mask |= NESButtons::Left;
    if (SDL_GameControllerGetButton(pad, SDL_CONTROLLER_BUTTON_DPAD_RIGHT))
      p2Mask |= NESButtons::Right;
  }

  // socd cleaning
  if ((p1Mask & NESButtons::Left) && (p1Mask & NESButtons::Right))
    p1Mask &= ~(NESButtons::Left | NESButtons::Right);
  if ((p1Mask & NESButtons::Up) && (p1Mask & NESButtons::Down))
    p1Mask &= ~(NESButtons::Up | NESButtons::Down);
  if ((p2Mask & NESButtons::Left) && (p2Mask & NESButtons::Right))
    p2Mask &= ~(NESButtons::Left | NESButtons::Right);
  if ((p2Mask & NESButtons::Up) && (p2Mask & NESButtons::Down))
    p2Mask &= ~(NESButtons::Up | NESButtons::Down);

  memory.m_p1InputState = p1Mask;
  memory.m_p2InputState = p2Mask;
}
