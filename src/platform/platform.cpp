#include "platform/platform.h"
#include "apu/apu.h"
#include "cpu/disassembler.h"
#include "cpu/operations.h"
#include "imgui.h"
#include "ppu/ppu.h"
#include <SDL2/SDL_audio.h>

#include <SDL2/SDL_log.h>
#include <algorithm>
#include <cfloat>

constexpr u32 TARGET_BUFFER_SIZE = 735 * sizeof(float) * 3;

void Platform::Update(void const *buffer, int pitch, std::shared_ptr<CPU> cpu) {
  ImGuiIO &io = ImGui::GetIO();
  if (!io.WantCaptureKeyboard) {
    m_inputManager.pollInputs(cpu->memory());
  } else {
    cpu->memory().m_p1InputState = 0;
  }
  SDL_UpdateTexture(texture, nullptr, buffer, pitch);
  SDL_RenderClear(renderer);
  SDL_RenderCopy(renderer, texture, nullptr, nullptr);
  ImGui_ImplSDLRenderer2_NewFrame();
  ImGui_ImplSDL2_NewFrame();
  ImGui::NewFrame();
  if (m_isDebug) {
    drawObjectMemory(cpu);
    drawNametable(cpu);
    drawCpuState(cpu);
    drawMemoryInspector(cpu);
    drawAudioWaves(cpu->memory().m_apu, m_lastAudioFrame);
    drawCpuDisassembler(cpu);
  }
  ImGui::Render();
  ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData(), renderer);
  if (m_audioDevice != 0 && !m_isPaused) {

    while (SDL_GetQueuedAudioSize(m_audioDevice) > TARGET_BUFFER_SIZE) {
      SDL_Delay(1);
    }

    auto &frameAudio = cpu->memory().m_apu->getFrameAudio();
    if (m_isDebug) {
    }

    if (frameAudio.sampleCount > 0) {
      SDL_QueueAudio(m_audioDevice, frameAudio.master,
                     frameAudio.sampleCount * sizeof(float));
      m_lastAudioFrame = frameAudio;
      frameAudio.clear();
    } else {
      SDL_LogWarn(SDL_LOG_CATEGORY_AUDIO, "audio buffer empty");
    }
  }
  SDL_RenderPresent(renderer);
}

void Platform::drawObjectMemory(std::shared_ptr<CPU> cpu) {
  ImGui::Begin("mbdnes Object Memory Inspector", nullptr,
               ImGuiWindowFlags_AlwaysAutoResize);
  std::shared_ptr<Mapper> activeMapper = cpu->memory().getMapper();

  if (activeMapper != nullptr) {
    const float pixelScale = 2.0f;
    ImDrawList *drawList = ImGui::GetWindowDrawList();
    ImVec2 screenCursor = ImGui::GetCursorScreenPos();
    ImGui::Text("Pattern Table 0 Font Tile Sheet (0x0000 - 0x0FFF):");
    for (int tileIdx = 0; tileIdx < 256; ++tileIdx) {
      float tileX = screenCursor.x + (tileIdx % 16) * (8 * pixelScale + 4);
      float tileY = screenCursor.y + (tileIdx / 16) * (8 * pixelScale + 4);
      for (int row = 0; row < 8; ++row) {
        u16 addressLow = (tileIdx * 16) + row;
        u16 addressHigh = addressLow + 8;

        u8 byteLow = activeMapper->ppuRead(addressLow);
        u8 byteHigh = activeMapper->ppuRead(addressHigh);
        for (int col = 0; col < 8; ++col) {
          u8 bit0 = (byteLow >> (7 - col)) & 0x01;
          u8 bit1 = (byteHigh >> (7 - col)) & 0x01;
          u8 colorPixelIndex = (bit1 << 1) | bit0;
          if (colorPixelIndex == 0)
            continue;
          ImU32 drawingColor = IM_COL32(85, 85, 85, 255);
          if (colorPixelIndex == 2)
            drawingColor = IM_COL32(170, 170, 170, 255);
          if (colorPixelIndex == 3)
            drawingColor = IM_COL32(255, 255, 255, 255);

          float pX = tileX + col * pixelScale;
          float pY = tileY + row * pixelScale;

          drawList->AddRectFilled(ImVec2(pX, pY),
                                  ImVec2(pX + pixelScale, pY + pixelScale),
                                  drawingColor);
        }
      }
    }

    ImGui::Dummy(ImVec2(16 * (8 * pixelScale + 4), 16 * (8 * pixelScale + 4)));
    ImGui::Dummy(ImVec2(16 * (8 * pixelScale + 4), 16 * (8 * pixelScale + 4)));
    ImGui::Separator();

    // Render Pattern Table 1
    ImGui::Text("Pattern Table 1 Sprite Sheet (0x1000 - 0x1FFF):");
    ImDrawList *drawList1 = ImGui::GetWindowDrawList();
    ImVec2 screenCursor1 = ImGui::GetCursorScreenPos();
    for (int tileIdx = 0; tileIdx < 256; ++tileIdx) {
      float tileX = screenCursor1.x + (tileIdx % 16) * (8 * pixelScale + 4);
      float tileY = screenCursor1.y + (tileIdx / 16) * (8 * pixelScale + 4);
      for (int row = 0; row < 8; ++row) {
        u16 addressLow = 0x1000 + (tileIdx * 16) + row;
        u16 addressHigh = addressLow + 8;

        u8 byteLow = activeMapper->ppuRead(addressLow);
        u8 byteHigh = activeMapper->ppuRead(addressHigh);
        for (int col = 0; col < 8; ++col) {
          u8 bit0 = (byteLow >> (7 - col)) & 0x01;
          u8 bit1 = (byteHigh >> (7 - col)) & 0x01;
          u8 colorPixelIndex = (bit1 << 1) | bit0;
          if (colorPixelIndex == 0)
            continue;
          ImU32 drawingColor = IM_COL32(85, 85, 85, 255);
          if (colorPixelIndex == 2)
            drawingColor = IM_COL32(170, 170, 170, 255);
          if (colorPixelIndex == 3)
            drawingColor = IM_COL32(255, 255, 255, 255);

          float pX = tileX + col * pixelScale;
          float pY = tileY + row * pixelScale;
          drawList1->AddRectFilled(ImVec2(pX, pY),
                                   ImVec2(pX + pixelScale, pY + pixelScale),
                                   drawingColor);
        }
      }
    }
    ImGui::Dummy(ImVec2(16 * (8 * pixelScale + 4), 16 * (8 * pixelScale + 4)));
  } else {
    ImGui::TextColored(ImVec4(1.0f, 0.3f, 0.3f, 1.0f),
                       "Warning: No Active Cartridge Mapper Intercept Found!");
  }
  ImGui::End();
}

void Platform::drawNametable(std::shared_ptr<CPU> cpu) {
  ImGui::Begin("mbdnes Nametable Inspector");
  if (cpu->memory().m_ppu) {
    auto ppu = cpu->memory().m_ppu;

    if (ImGui::BeginTabBar("NametableTabs")) {
      u16 baseAddresses[4] = {0x2000, 0x2400, 0x2800, 0x2C00};
      for (int nt = 0; nt < 4; ++nt) {
        char tabLabel[32];
        snprintf(tabLabel, sizeof(tabLabel), "Nametable %d ($%04X)", nt,
                 baseAddresses[nt]);

        if (ImGui::BeginTabItem(tabLabel)) {
          ImGui::BeginChild("NtGrid", ImVec2(0, 0), true,
                            ImGuiWindowFlags_HorizontalScrollbar);

          for (int row = 0; row < 30; ++row) {
            for (int col = 0; col < 32; ++col) {
              u16 targetVramAddr = baseAddresses[nt] + (row * 32 + col);
              u8 tileId = ppu->memory().read(targetVramAddr);

              if (tileId != 0x00) {
                ImGui::Text("%02X", tileId);
              } else {
                ImGui::TextColored(ImVec4(0.3f, 0.3f, 0.3f, 0.4f), "..");
              }
              if (col < 31) {
                ImGui::SameLine();
              }
            }
          }
          ImGui::EndChild();
          ImGui::EndTabItem();
        }
      }
      ImGui::EndTabBar();
    }
  } else {
    ImGui::TextColored(ImVec4(1.0f, 0.3f, 0.3f, 1.0f),
                       "Warning: CPU has no PPU");
  }
  ImGui::End();
}

void Platform::drawCpuState(std::shared_ptr<CPU> cpu) {
  ImGui::Begin("mbdnes CPU/PPU State", nullptr,
               ImGuiWindowFlags_AlwaysAutoResize);

  if (m_isPaused) {
    if (ImGui::Button(" Run ")) {
      m_isPaused = false;
    }
    ImGui::SameLine();
    if (ImGui::Button(" Step ")) {
      cpu->step();
    }
  } else {
    if (ImGui::Button(" Pause ")) {
      m_isPaused = true;
      if (m_audioDevice != 0)
        SDL_ClearQueuedAudio(m_audioDevice);
    }
  }
  ImGui::SeparatorText("State");

  ImGui::Text("PC:      $%04X", cpu->m_pc.getContent());
  ImGui::Text("A:       $%02X", cpu->m_a.getContent());
  ImGui::Text("X:       $%02X", cpu->m_x.getContent());
  ImGui::Text("Y:       $%02X", cpu->m_y.getContent());
  ImGui::Text("SP:      $%02X", cpu->m_sp.getContent());
  ImGui::Text("Cycles:  %zu", cpu->m_cycle);

  ImGui::SeparatorText("Flags");
  auto drawFlag = [](const char *name, bool status) {
    if (status) {
      ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "[%s]", name);
    } else {
      ImGui::TextColored(ImVec4(0.4f, 0.4f, 0.4f, 1.0f), "[%s]", name);
    }
    if (*name != 'C')
      ImGui::SameLine();
  };

  drawFlag("N", cpu->m_flags.m_n);
  drawFlag("V", cpu->m_flags.m_v);
  drawFlag("-", true);
  drawFlag("B", cpu->m_flags.m_b);
  drawFlag("D", cpu->m_flags.m_d);
  drawFlag("I", cpu->m_flags.m_i);
  drawFlag("Z", cpu->m_flags.m_z);
  drawFlag("C", cpu->m_flags.m_c);
  ImGui::SeparatorText("PPU");
  ImGui::Text("Scanline:      %d", cpu->memory().m_ppu->m_scanline);
  ImGui::Text("Cycles:        %d", cpu->memory().m_ppu->m_cycle);
  ImGui::Text("Coarse x:      %d",
              cpu->memory().m_ppu->m_loopy.m_vAddress.m_coarseX);
  ImGui::Text("Fine x:        %d", cpu->memory().m_ppu->m_loopy.m_fineX);
  ImGui::Text("Fine y:        %d", cpu->memory().m_ppu->m_loopy.m_fineX);
  ImGui::NewLine();

  ImGui::End();
}

void Platform::drawMemoryInspector(std::shared_ptr<CPU> cpu) {
  ImGui::Begin("mbdnes Memory Inspector", nullptr,
               ImGuiWindowFlags_HorizontalScrollbar);

  static bool scrollToAddress = false;
  static u16 jumpTo = 0x0000;

  static char inputBuffer[5] = "";

  if (ImGui::Button("Zero Page ($0000)")) {
    jumpTo = 0x0000;
    scrollToAddress = true;
  }
  ImGui::SameLine();
  if (ImGui::Button("Vectors ($FFFA)")) {
    jumpTo = 0xfff0;
    scrollToAddress = true;
  }
  if (ImGui::Button("PRG ROM ($8000)")) {
    jumpTo = 0x8000;
    scrollToAddress = true;
  }
  ImGui::SameLine();
  if (ImGui::Button("Stack")) {
    jumpTo = 0x0100 | (cpu->m_sp.getContent() & 0xff);
    scrollToAddress = true;
  }
  ImGui::SameLine();
  if (ImGui::Button("PC")) {
    jumpTo = (cpu->m_pc.getContent() & 0xffff);
    scrollToAddress = true;
  }

  ImGui::Separator();
  ImGui::SetNextItemWidth(60.0f);
  bool enterPressed =
      ImGui::InputText("##HexJump", inputBuffer, IM_ARRAYSIZE(inputBuffer),
                       ImGuiInputTextFlags_CharsHexadecimal |
                           ImGuiInputTextFlags_EnterReturnsTrue);
  ImGui::SameLine();
  bool goClicked = ImGui::Button("Go");

  if ((enterPressed || goClicked) && inputBuffer[0] != '\0') {
    u16 parsedAddr = 0;
    if (sscanf(inputBuffer, "%hx", &parsedAddr) == 1) {
      jumpTo = parsedAddr & 0xffff;
      scrollToAddress = true;
    }
  }
  ImGui::SameLine();
  ImGui::TextDisabled("(hex)");
  ImGui::Separator();

  ImGui::BeginChild("VirtualizedHexGrid", ImVec2(0, 450), true,
                    ImGuiWindowFlags_HorizontalScrollbar);

  const int totalRows = 4096;
  ImGuiListClipper clipper;
  clipper.Begin(totalRows, ImGui::GetTextLineHeightWithSpacing());
  if (scrollToAddress) {
    int targetRow = jumpTo / 16;
    ImGui::SetScrollFromPosY(
        ImGui::GetCursorStartPos().y +
        (targetRow * ImGui::GetTextLineHeightWithSpacing()));
    scrollToAddress = false;
  }
  ImDrawList *drawList = ImGui::GetWindowDrawList();

  while (clipper.Step()) {
    for (int rowIdx = clipper.DisplayStart; rowIdx < clipper.DisplayEnd;
         ++rowIdx) {
      u16 rowAddr = static_cast<u16>(rowIdx * 16);
      ImGui::TextColored(ImVec4(0.4f, 0.7f, 1.0f, 1.0f), "$%04X: ", rowAddr);
      ImGui::SameLine();

      for (u16 col = 0; col < 16; ++col) {
        u16 targetAddr = rowAddr + col;
        u8 byteValue = cpu->memory().read(targetAddr);

        bool isPCByte = (targetAddr == cpu->m_pc.getContent());
        bool isBreakpoint =
            (m_breakpointAddress == static_cast<int>(targetAddr));

        ImVec2 posMin = ImGui::GetCursorScreenPos();
        float cellWidth = ImGui::CalcTextSize("00").x;
        float cellHeight = ImGui::GetTextLineHeight();
        ImVec2 posMax = ImVec2(posMin.x + cellWidth, posMin.y + cellHeight);

        if (isBreakpoint) {
          drawList->AddRectFilled(posMin, posMax, IM_COL32(255, 50, 50, 80));
        }
        if (isPCByte) {
          drawList->AddRectFilled(posMin, posMax, IM_COL32(50, 255, 50, 90));
        }

        if (byteValue != 0x00) {
          ImGui::Text("%02X", byteValue);
        } else {
          ImGui::TextColored(ImVec4(0.3f, 0.3f, 0.3f, 1.0f), "00");
        }

        if (ImGui::IsItemHovered()) {
          ImGui::SetTooltip("Click to toggle breakpoint at $%04X", targetAddr);
          if (ImGui::IsMouseClicked(0)) {
            if (m_breakpointAddress == static_cast<int>(targetAddr)) {
              m_breakpointAddress = -1;
              SDL_LogDebug(SDL_LOG_CATEGORY_SYSTEM,
                           "[BREAKPOINT]: Removed from $%04X\n", targetAddr);
            } else {
              m_breakpointAddress = static_cast<int>(targetAddr);
              SDL_LogDebug(SDL_LOG_CATEGORY_SYSTEM,
                           "[BREAKPOINT]: Set at $%04X\n", targetAddr);
            }
          }
        }

        if (col < 15)
          ImGui::SameLine();
      }
    }
  }
  clipper.End();

  ImGui::EndChild();
  ImGui::End();
}

void Platform::drawAudioWaves(std::shared_ptr<APU> apu,
                              FrameAudioCapture &frameAudio) {
  if (!apu)
    return;

  ImGui::Begin("mbdnes APU Audio Frame Analyzer");

  bool is5Step = apu->m_registers.m_apuFrameCounter.use5StepSequencer;
  ImGui::Text("Frame Sequencer: %s Mode | Samples This Frame: %d",
              is5Step ? "5-Step" : "4-Step", frameAudio.sampleCount);
  ImGui::Separator();

  int displayCount = frameAudio.sampleCount > 0 ? frameAudio.sampleCount : 1;

  auto drawFrameScope = [&](const char *label, float *buffer, ImVec4 plotColor,
                            bool *muted) {
    static size_t numPlots = 0;
    ImGui::BeginGroup();

    if (muted != nullptr) {
      ImGui::Checkbox((std::string("Mute ") + label).c_str(), muted);
    }

    ImGui::PushStyleColor(ImGuiCol_PlotLines, plotColor);
    auto sensibleLabel = std::string(label);
    std::replace(sensibleLabel.begin(), sensibleLabel.end(), ' ', '_');
    std::string uniquePlotId = std::string("#Plot_") + sensibleLabel;
    ImGui::PlotLines(uniquePlotId.c_str(), buffer, displayCount, 0, nullptr,
                     FLT_MAX, FLT_MAX,
                     ImVec2(ImGui::GetContentRegionAvail().x, 50.0f));
    ImGui::PopStyleColor();

    ImGui::EndGroup();
    ImGui::Spacing();
  };

  drawFrameScope("Master", frameAudio.master, ImVec4(1.0f, 1.0f, 1.0f, 1.0f),
                 nullptr);
  ImGui::Separator();

  drawFrameScope("Pulse 1", frameAudio.pulse1, ImVec4(0.3f, 0.6f, 1.0f, 1.0f),
                 &m_mutePulse1);

  drawFrameScope("Pulse 2", frameAudio.pulse2, ImVec4(0.3f, 0.6f, 1.0f, 1.0f),
                 &m_mutePulse2);

  drawFrameScope("Triangle", frameAudio.triangle,
                 ImVec4(0.2f, 0.9f, 0.4f, 1.0f), &m_muteTriangle);

  drawFrameScope("Noise", frameAudio.noise, ImVec4(1.0f, 0.6f, 0.2f, 1.0f),
                 &m_muteNoise);

  drawFrameScope("DMC", frameAudio.dmc, ImVec4(0.8f, 0.4f, 0.8f, 1.0f),
                 &m_muteDmc);

  ImGui::End();
}

void Platform::drawCpuDisassembler(std::shared_ptr<CPU> cpu) {
  ImGui::Begin("mbdnes CPU Disassembler", nullptr, ImGuiWindowFlags_None);

  static bool trackPC = true;
  ImGui::Checkbox("Auto-scroll to PC", &trackPC);
  ImGui::Separator();

  if (!m_disasmCacheValid || m_disasmAddresses.empty()) {
    m_disasmAddresses.clear();
    int scanAddr = 0;
    while (scanAddr < 65536) {
      m_disasmAddresses.push_back(static_cast<u16>(scanAddr));
      u8 opc = cpu->memory().read(scanAddr);
      u8 size = OPERATIONS[opc].addressingMode
                    ? OPERATIONS[opc].addressingMode->m_inputSize
                    : 0;
      scanAddr += 1 + size;
    }
    m_disasmCacheValid = true;
  }

  ImGui::BeginChild("VirtualizedDisassemblyGrid", ImVec2(0, 0), true,
                    ImGuiWindowFlags_HorizontalScrollbar);

  ImGuiListClipper clipper;
  clipper.Begin(static_cast<int>(m_disasmAddresses.size()),
                ImGui::GetTextLineHeightWithSpacing());

  u16 currentPC = cpu->m_pc.getContent();

  int pcIndex = -1;
  if (trackPC) {
    for (size_t i = 0; i < m_disasmAddresses.size(); ++i) {
      if (m_disasmAddresses[i] == currentPC) {
        pcIndex = static_cast<int>(i);
        break;
      }
    }
    if (pcIndex != -1) {
      ImGui::SetScrollFromPosY(
          ImGui::GetCursorStartPos().y +
              (pcIndex * ImGui::GetTextLineHeightWithSpacing()),
          0.5f);
    }
  }

  ImDrawList *drawList = ImGui::GetWindowDrawList();

  while (clipper.Step()) {
    for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; ++i) {
      u16 targetAddr = m_disasmAddresses[i];
      u8 opcode = cpu->memory().read(targetAddr);
      const auto &op = OPERATIONS[opcode];

      bool isCurrentPC = (targetAddr == currentPC);
      bool isBreakpoint = (m_breakpointAddress == static_cast<int>(targetAddr));

      if (isCurrentPC || isBreakpoint) {
        ImVec2 posMin = ImGui::GetCursorScreenPos();
        float rowWidth = ImGui::GetContentRegionAvail().x;
        float rowHeight = ImGui::GetTextLineHeight();
        ImVec2 posMax = ImVec2(posMin.x + rowWidth, posMin.y + rowHeight);

        if (isBreakpoint) {
          drawList->AddRectFilled(posMin, posMax, IM_COL32(255, 50, 50, 80));
        }
        if (isCurrentPC) {
          drawList->AddRectFilled(posMin, posMax, IM_COL32(50, 255, 50, 60));
        }
      }

      if (isCurrentPC) {
        ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f),
                           "=> $%04X: ", targetAddr);
      } else if (isBreakpoint) {
        ImGui::TextColored(ImVec4(1.0f, 0.4f, 0.4f, 1.0f),
                           " B  $%04X: ", targetAddr);
      } else {
        ImGui::TextColored(ImVec4(0.4f, 0.7f, 1.0f, 1.0f),
                           "    $%04X: ", targetAddr);
      }
      ImGui::SameLine();

      u16 operandValue = 0;
      u8 size = 0;
      if (op.addressingMode) {
        size = op.addressingMode->m_inputSize;
        if (size == 1) {
          operandValue = cpu->memory().read(targetAddr + 1);
        } else if (size == 2) {
          u16 low = cpu->memory().read(targetAddr + 1);
          u16 high = cpu->memory().read(targetAddr + 2);
          operandValue = (high << 8) | low;
        }
      }

      if (size == 0) {
        ImGui::TextDisabled("%02X       ", opcode);
      } else if (size == 1) {
        ImGui::TextDisabled("%02X %02X    ", opcode,
                            static_cast<u8>(operandValue & 0xFF));
      } else {
        ImGui::TextDisabled("%02X %02X %02X ", opcode,
                            static_cast<u8>(operandValue & 0xFF),
                            static_cast<u8>(operandValue >> 8));
      }
      ImGui::SameLine();

      char disassemblyBuffer[64] = {0};
      disassembleInstructionTo(disassemblyBuffer, opcode, operandValue,
                               targetAddr);

      ImGui::Text("%s", disassemblyBuffer);

      if (ImGui::IsItemHovered() ||
          (ImGui::IsWindowHovered() &&
           ImGui::IsMouseHoveringRect(ImGui::GetItemRectMin(),
                                      ImGui::GetItemRectMax()))) {
        ImGui::SetTooltip("Click row to toggle execution breakpoint at $%04X",
                          targetAddr);
        if (ImGui::IsMouseClicked(0)) {
          if (m_breakpointAddress == static_cast<int>(targetAddr)) {
            m_breakpointAddress = -1;
          } else {
            m_breakpointAddress = static_cast<int>(targetAddr);
          }
        }
      }
    }
  }
  clipper.End();

  ImGui::EndChild();
  ImGui::End();
}
