#include "apu/apu.h"
#include "cpu/cpu.h"
#include "cpu/interrupts.h"
#include "platform/platform.h"
#include "ppu/ppu.h"
#include "rom_loader.h"

#include <SDL2/SDL_audio.h>
#include <SDL2/SDL_log.h>
#include <filesystem>
#include <thread>

#define PPU_STEPS_PER_CPU_CYCLE 3

namespace fs = std::filesystem;

// TODO: clean this up; several things belong in Platform, several things
// can be abstracted, and none of it is pretty
int main() {
  Platform platform{"mbdnes", 1280, 960, 256, 240};
  CPUMemory cpuMemory{};

  auto cpu = std::make_shared<CPU>(cpuMemory);
  auto ppu = std::make_shared<PPU>(*cpu);
  auto apu = std::make_shared<APU>(cpu.get());

#ifdef DEFAULT_SDL_LOG_LEVEL
  SDL_LogSetAllPriority(static_cast<SDL_LogPriority>(DEFAULT_SDL_LOG_LEVEL));
#endif

  std::shared_ptr<Cartridge> cartridge = nullptr;
  std::shared_ptr<Mapper> mapper = nullptr;
  bool isCartridgeLoaded = false;

  u8 cpuCycles = 0;
  int unitCycles = 0;
  auto onIntr = [&cpu, &unitCycles, &cpuCycles](Interrupt i) -> void {
    auto newCpuCycles = cpu->interrupt(i);
    cpuCycles += newCpuCycles;
    unitCycles += newCpuCycles * PPU_STEPS_PER_CPU_CYCLE;
  };

  std::vector<u32> frameBuffer{256 * 240, 0};
  bool isRunning = true;
  auto pendingPPUCycles = 0;
  u8 keys[17] = {0};

  const std::chrono::microseconds targetFrameDuration(16640);
  auto nextFrameTargetTime = std::chrono::high_resolution_clock::now();

  auto onFrame = [&frameBuffer](std::vector<u32> &newFrameBuffer) -> void {
    if (frameBuffer.size() != newFrameBuffer.size()) {
      frameBuffer.resize(newFrameBuffer.size());
    }
    std::copy(newFrameBuffer.begin(), newFrameBuffer.end(),
              frameBuffer.begin());
  };

  fs::path currentPath = fs::current_path();

  while (isRunning) {
    AudioOverrides overrides = {.mutePulse1 = &platform.m_mutePulse1,
                                .mutePulse2 = &platform.m_mutePulse2,
                                .muteTriangle = &platform.m_muteTriangle,
                                .muteNoise = &platform.m_muteNoise,
                                .muteDmc = &platform.m_muteDmc};
    isRunning = !platform.ProcessInput(keys);

    if (!isCartridgeLoaded) {
      SDL_RenderClear(platform.renderer);

      ImGui_ImplSDLRenderer2_NewFrame();
      ImGui_ImplSDL2_NewFrame();
      ImGui::NewFrame();

      ImGui::SetNextWindowPos(ImVec2(200, 180), ImGuiCond_FirstUseEver);
      ImGui::SetNextWindowSize(ImVec2(880, 600), ImGuiCond_FirstUseEver);

      ImGui::Begin("mbdnes", nullptr,
                   ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
                       ImGuiWindowFlags_NoMove);

      ImGui::TextColored(ImVec4(0.4f, 0.7f, 1.0f, 1.0f),
                         "Current Directory: %s", currentPath.string().c_str());
      ImGui::Separator();

      ImGui::BeginChild("SelectableFileListZone", ImVec2(0, -40), true);

      if (currentPath.has_parent_path()) {
        if (ImGui::Selectable(" [..] Up to Parent Directory", false,
                              ImGuiSelectableFlags_AllowDoubleClick)) {
          if (ImGui::IsMouseDoubleClicked(0)) {
            currentPath = currentPath.parent_path();
          }
        }
      }

      try {
        for (const auto &entry : fs::directory_iterator(currentPath)) {
          std::string name = entry.path().filename().string();

          if (entry.is_directory()) {
            if (ImGui::Selectable((std::string("> ") + name).c_str(), false,
                                  ImGuiSelectableFlags_AllowDoubleClick)) {
              if (ImGui::IsMouseDoubleClicked(0)) {
                currentPath = entry.path();
                break;
              }
            }
          } else {
            if (entry.path().extension() == ".nes") {
              if (ImGui::Selectable((std::string("./") + name).c_str(), false,
                                    ImGuiSelectableFlags_AllowDoubleClick)) {
                if (ImGui::IsMouseDoubleClicked(0)) {
                  try {
                    cartridge = ROMLoader::loadFromFile(entry.path().string());
                    ppu->memory().onLoad(cartridge, nullptr);
                    mapper = createMapper(cpu, ppu, cartridge);

                    cpu->memory().onLoad(ppu, apu, mapper);
                    ppu->onLoad(mapper);
                    ppu->memory().onLoad(cartridge, mapper);
                    mapper->cpuWrite(0x8000, 0);

                    cpu->interrupt(INTERRUPTS[Interrupts::RESET]);

                    SDL_LogInfo(
                        SDL_LOG_CATEGORY_APPLICATION,
                        "[LOAD SUCCESS]: Booting iNES Mapper %d Target: %s",
                        cartridge->m_header.mapperId, name.c_str());

                    isCartridgeLoaded = true;
                  } catch (const std::exception &ex) {
                    SDL_LogError(
                        SDL_LOG_CATEGORY_ERROR,
                        "ROM Loader failed to parse chosen image payload: %s",
                        ex.what());
                  }
                  break;
                }
              }
            }
          }
        }
      } catch (...) {
        ImGui::TextColored(ImVec4(1.0f, 0.4f, 0.4f, 1.0f),
                           "Access Denied or Directory Unreadable.");
      }

      ImGui::EndChild();
      ImGui::TextDisabled("Double-click a directory to open it, or "
                          "double-click a .nes file to launch.");
      ImGui::End();

      ImGui::Render();
      ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData(),
                                            platform.renderer);
      SDL_RenderPresent(platform.renderer);

      std::this_thread::sleep_for(std::chrono::milliseconds(8));
      continue;
    }

    if (!platform.m_isPaused) {
      try {
        auto currentFrame = ppu->m_frame;
        while (ppu->m_frame == currentFrame) {
          if (platform.m_breakpointAddress != -1) {
            u16 target16 =
                static_cast<u16>(platform.m_breakpointAddress & 0xffff);

            if (cpu->m_pc.getContent() == target16) {
              SDL_LogDebug(SDL_LOG_CATEGORY_SYSTEM,
                           "[BREAKPOINT HIT]: Halted execution right before "
                           "instruction at $%04X",
                           target16);

              platform.m_isPaused = true;
              break;
            }
          }

          cpuCycles = cpu->step();

          for (u8 c = 0; c < cpuCycles; c++) {
            apu->step(nullptr, &overrides);
          }

          const auto scanline = ppu->m_scanline;
          unitCycles = pendingPPUCycles + cpuCycles * PPU_STEPS_PER_CPU_CYCLE;
          pendingPPUCycles = 0;

          for (auto i = 0; i < unitCycles; i++) {
            if (ppu->m_sprite0HitCycleTarget != -1 &&
                ppu->m_cycle >= ppu->m_sprite0HitCycleTarget) {
              ppu->m_registers.m_ppuStatus.setSprite0Hit(true);
              ppu->m_sprite0HitCycleTarget = -1;
            }
            if (ppu->m_cycle == 260 && ppu->m_scanline >= 0 &&
                ppu->m_scanline < 240 &&
                ppu->m_registers.m_ppuMask.isRenderingEnabled()) {
              if (mapper)
                mapper->tick();
            }

            if ((ppu->m_cycle > 1 && ppu->m_cycle < 256) ||
                (ppu->m_cycle > 260 && ppu->m_cycle < 304) ||
                (ppu->m_cycle > 304 && ppu->m_cycle < 340)) {
              if (ppu->m_scanline == 241 && ppu->m_cycle == 1) {
                ppu->m_registers.m_ppuStatus.setVBlank(true);
                if (ppu->m_registers.m_ppuCtrl.generateNMIOnVBlank()) {
                  onIntr(INTERRUPTS[Interrupts::NMI]);
                }
              }
              ppu->m_cycle++;
              continue;
            }
            const auto wasInVBlankInterval =
                ppu->m_registers.m_ppuStatus.isInVBlank();

            ppu->step(onFrame, onIntr);

            const auto isInVBlankInterval =
                ppu->m_registers.m_ppuStatus.isInVBlank();

            if (!wasInVBlankInterval && isInVBlankInterval) {
              pendingPPUCycles = unitCycles - i - 1;
              break;
            }
          }
        }
        nextFrameTargetTime += targetFrameDuration;
        auto currentTime = std::chrono::high_resolution_clock::now();
        if (currentTime < nextFrameTargetTime) {
          if (nextFrameTargetTime - currentTime >
              std::chrono::milliseconds(2)) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
          }
          while (std::chrono::high_resolution_clock::now() <
                 nextFrameTargetTime) {
#if defined(__x86_64__) || defined(_M_X64) || defined(__i386__) ||             \
    defined(_M_IX86)
#if defined(_MSC_VER)
            _mm_pause();
#else
            asm volatile("pause" ::: "memory");
#endif
#elif defined(__ARM_ARCH) || defined(__aarch64__) || defined(_M_ARM64)
            asm volatile("yield" ::: "memory");
#endif
          }
        } else {
          if (currentTime - nextFrameTargetTime > targetFrameDuration * 2) {
            SDL_LogDebug(SDL_LOG_CATEGORY_VIDEO, "video behind target");
            nextFrameTargetTime = currentTime;
          }
        }
      } catch (const std::runtime_error &ex) {
        SDL_LogCritical(SDL_LOG_CATEGORY_ERROR, "\n[CPU CRASH]: %s", ex.what());
        cpu->dumpExecutionHistory();
        platform.m_isPaused = true;
      }
    } else {
      std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    platform.Update(frameBuffer.data(), 256 * sizeof(u32), cpu);
  }
  return 0;
}
