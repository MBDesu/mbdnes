#pragma once
#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_log.h>
#ifndef MODULE_PLATFORM_H
#define MODULE_PLATFORM_H

#include "SDL2/SDL.h"
#include "cpu/cpu.h"
#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_sdlrenderer2.h"
#include "platform/controller.h"
#include <SDL2/SDL_audio.h>

class PPU;

struct FrameAudioCapture {
  // (44100 / 60) = 735 samples
  static constexpr int MAX_FRAME_SAMPLES = 800;

  float master[MAX_FRAME_SAMPLES] = {0};
  float pulse1[MAX_FRAME_SAMPLES] = {0};
  float pulse2[MAX_FRAME_SAMPLES] = {0};
  float triangle[MAX_FRAME_SAMPLES] = {0};
  float noise[MAX_FRAME_SAMPLES] = {0};
  float dmc[MAX_FRAME_SAMPLES] = {0};

  int sampleCount = 0;

  void clear() { sampleCount = 0; }

  void append(float m, float p1, float p2, float tri, float n, float d) {
    if (sampleCount < MAX_FRAME_SAMPLES) {
      master[sampleCount] = m;
      pulse1[sampleCount] = p1;
      pulse2[sampleCount] = p2;
      triangle[sampleCount] = tri;
      noise[sampleCount] = n;
      dmc[sampleCount] = d;
      sampleCount++;
    }
  }
};

class Platform {
public:
  SDL_Renderer *renderer{};
  SDL_AudioDeviceID m_audioDevice = 0;
  bool m_isPaused = true;
  bool m_isDebug = false;
  int m_breakpointAddress = -1;
  FrameAudioCapture m_frameAudio;
  bool m_mutePulse1 = false;
  bool m_mutePulse2 = false;
  bool m_muteTriangle = false;
  bool m_muteNoise = false;
  bool m_muteDmc = false;

  Platform(char const *title, int windowWidth, int windowHeight,
           int textureWidth, int textureHeight) {
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Init(SDL_INIT_AUDIO);
    SDL_Init(SDL_INIT_GAMECONTROLLER);

    window = SDL_CreateWindow(title, 0, 0, windowWidth, windowHeight,
                              SDL_WINDOW_SHOWN);

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888,
                                SDL_TEXTUREACCESS_STREAMING, textureWidth,
                                textureHeight);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();

    ImGui_ImplSDL2_InitForSDLRenderer(window, renderer);
    ImGui_ImplSDLRenderer2_Init(renderer);
    InitAudio();
  }

  ~Platform() {
    ImGui_ImplSDLRenderer2_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
    if (m_audioDevice != 0)
      SDL_CloseAudioDevice(m_audioDevice);
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
  }

  void Update(void const *buffer, int pitch, std::shared_ptr<CPU> cpu);

  bool ProcessInput(uint8_t *keys) {
    bool quit = false;
    SDL_Event event;

    while (SDL_PollEvent(&event)) {
      ImGui_ImplSDL2_ProcessEvent(&event);

      switch (event.type) {
      case SDL_QUIT: {
        quit = true;
      } break;

      case SDL_KEYDOWN: {
        switch (event.key.keysym.sym) {
        case SDLK_ESCAPE: {
          quit = true;
          break;
        }
        case SDLK_TAB: {
          keys[0x10] = 1;
          m_isDebug = !m_isDebug;
          break;
        }
        default:
          break;
        }
      } break;
      case SDL_KEYUP: {
        switch (event.key.keysym.sym) {
        case SDLK_TAB: {
          keys[0x10] = 0;
          break;
        }
        default:
          break;
        }
      }
      default:
        m_inputManager.handleEvent(event);
        break;
      }
    }

    return quit;
  }

  void InitAudio() {
    SDL_AudioSpec targetSpec;
    SDL_zero(targetSpec);

    targetSpec.freq = 44100;
    targetSpec.format = AUDIO_F32SYS;
    targetSpec.channels = 1;
    targetSpec.samples = 512;
    targetSpec.callback = nullptr;

    SDL_AudioSpec obtainedSpec;
    m_audioDevice =
        SDL_OpenAudioDevice(nullptr, 0, &targetSpec, &obtainedSpec, 0);

    if (m_audioDevice == 0) {
      SDL_LogError(SDL_LOG_CATEGORY_AUDIO,
                   "Failed to open SDL Audio Device: %s", SDL_GetError());
    } else {
      SDL_PauseAudioDevice(m_audioDevice, 0);
      SDL_LogInfo(SDL_LOG_CATEGORY_AUDIO,
                  "Opened output channel successfully at 44100Hz\n");
    }
  }

private:
  SDL_Window *window{};
  SDL_Texture *texture{};
  InputManager m_inputManager;
  FrameAudioCapture m_lastAudioFrame;

  void drawObjectMemory(std::shared_ptr<CPU> cpu);
  void drawNametable(std::shared_ptr<CPU> cpu);
  void drawCpuState(std::shared_ptr<CPU> cpu);
  void drawMemoryInspector(std::shared_ptr<CPU> cpu);
  void drawAudioWaves(std::shared_ptr<APU> apu, FrameAudioCapture &frameAudio);
};

#endif // MODULE_PLATFORM_H
