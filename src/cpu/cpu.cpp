#include "cpu/cpu.h"
// #include "cpu/disassembler.h"
#include "cpu/interrupts.h"
#include "cpu/operations.h"
#include "ppu/ppu.h"
#include <SDL2/SDL_log.h>

size_t CPU::addCycles(const Operation &operation) {
  const auto cycles = operation.cycles + m_extraCycles;
  m_cycle += cycles;
  m_extraCycles = 0;
  return cycles;
}

u8 CPU::interrupt(Interrupt interrupt, bool withBFlag) {
  // ignore IRQs when they're disabled
  if (interrupt.id == "IRQ" && m_flags.m_i) {
    return 0;
  }

  m_stack.push16(m_pc.getContent());

  u8 status = m_flags.getContent() | 0x20;

  // for BRK, bit 4 is set only on the stack copy
  if (withBFlag)
    status |= 0x10;
  else
    status &= ~0x10;

  m_stack.push(status);

  m_cycle += 7;
  m_flags.m_i = true;
  m_pc.setContent(m_memory.read16(interrupt.vector));
  return 7;
}

Operation CPU::fetchOperation() {
  const u8 opcode = m_memory.read(m_pc.getContent());
  const Operation operation = OPERATIONS[opcode];
  try {
    if (!operation.addressingMode)
      throw std::runtime_error("Invalid opcode");
    m_pc.increment();
  } catch (std::runtime_error ex) {
    if (opcode == 0x00) {
      SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION,
                   "Test Status Register ($6000): 0x%02X",
                   m_memory.read(0x6000));
      std::string testResult = "";
      for (u16 i = 0x6004; i < 0x6040; ++i) {
        char c = m_memory.read(i);
        if (c == 0)
          break;
        testResult += c;
      }
      SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, "Test Result String: %s",
                   testResult.c_str());
    }
    SDL_LogCritical(SDL_LOG_CATEGORY_ERROR, "%s: 0x%02x", ex.what(),
                    operation.id);
    dumpExecutionHistory();
  }
  return operation;
}

u16 CPU::fetchInput(const Operation &operation) {
  u16 input = 0;
  if (operation.addressingMode->m_inputSize == 1) {
    input = m_memory.read(m_pc.getContent());
    m_pc.increment();
  } else if (operation.addressingMode->m_inputSize == 2) {
    u16 lowByte = m_memory.read(m_pc.getContent());
    m_pc.increment();
    u16 highByte = m_memory.read(m_pc.getContent());
    m_pc.increment();
    input = (highByte << 8) | lowByte;
  }
  return input;
}

u8 CPU::step() {
  auto originalPc = m_pc.getContent();
  auto operation = fetchOperation();
  auto input = fetchInput(operation);

  CPULogFrame frame{
      .pc = originalPc,
      .opcode = operation.id,
      .instruction = {},
      .a = m_a.getContent(),
      .x = m_x.getContent(),
      .y = m_y.getContent(),
      .sp = m_sp.getContent(),
      .status = m_flags.getContent(),
      .cycles = m_cycle,
  };
  // u8 size = disassembleInstructionTo(frame.instruction, operation.id, input,
  //                                    originalPc);
  // printDisassembly(frame, size, input);
  m_executionHistory.push_back(frame);
  if (m_executionHistory.size() > MAX_LOG_FRAMES) {
    m_executionHistory.erase(m_executionHistory.begin());
  }

  if (operation.addressingMode->m_id == AddressingModeId::IMMEDIATE) {
    if (operation.instruction.runValue) {
      operation.instruction.runValue(*this, static_cast<u8>(input & 0xff));
    }
  } else if (operation.addressingMode->m_id == AddressingModeId::IMPLICIT) {
    if (operation.instruction.runNoArgs) {
      operation.instruction.runNoArgs(*this);
    }
  } else if (operation.instruction.runAddress) {
    u16 targetAddress = operation.addressingMode->getAddress(
        *this, input, operation.hasPageCrossPenalty);
    operation.instruction.runAddress(*this, targetAddress);
  } else if (operation.instruction.runValue) {
    u16 targetAddress = operation.addressingMode->getAddress(
        *this, input, operation.hasPageCrossPenalty);
    u8 value = m_memory.read(targetAddress);
    operation.instruction.runValue(*this, value);
  } else {
    u16 targetAddress = operation.addressingMode->getAddress(
        *this, input, operation.hasPageCrossPenalty);
    operation.instruction.run(*this, targetAddress);
  }
  return addCycles(operation);
}

void CPU::dumpExecutionHistory() const {
  SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,
              "\n=== mbdnes CRASH DIAGNOSTIC HISTORY LOG ===");
  SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,
              "%-6s | %-6s | %-2s | %-2s | %-2s | %-2s | %-2s | %s", "PC",
              "OPCODE", "A", "X", "Y", "SP", "ST", "TOTAL CYCLES");
  SDL_LogInfo(
      SDL_LOG_CATEGORY_APPLICATION,
      "-------------------------------------------------------------\n");

  for (const auto &frame : m_executionHistory) {

    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,
                "$%04X  | 0x%02X   | %02X | %02X | %02X | %02X | %02X | %llu",
                frame.pc, frame.opcode, frame.a, frame.x, frame.y, frame.sp,
                frame.status, frame.cycles);
  }
  SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,
              "===========================================");
}
