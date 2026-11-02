#include "apu/apu.h"
#include "cartridge.h"
#include "cpu/cpu.h"
#include "cpu/operations.h"
#include "globals.h"
#include "lib/reflection.h"
#include "mappers/0_NROM.h"
#include "ppu/ppu.h"

#include <array>
#include <catch2/catch_test_macros.hpp>
#include <memory>
#include <vector>

EXPOSE_PRIVATE_FIELD(CPUExecutionHistoryTag, CPU, std::vector<CPULogFrame>,
                     m_executionHistory);

std::shared_ptr<Cartridge> createMockCartridge(const std::vector<u8> &prgCode) {
  Header header;
  header.prgRomPages = 1;
  header.chrRomPages = 1;
  header.usesChrRam = false;
  header.has512BytePadding = false;
  header.mapperId = 0;
  header.mirroringId = "VERTICAL";
  header.submapperId = 0;

  size_t totalPrgSize = header.prgRomPages * 16384;
  size_t totalChrSize = header.chrRomPages * 8192;

  std::vector<u8> payloadBytes;
  payloadBytes.reserve(totalPrgSize + totalChrSize);

  payloadBytes.insert(payloadBytes.end(), prgCode.begin(), prgCode.end());
  payloadBytes.insert(payloadBytes.end(), totalPrgSize - prgCode.size(), 0x00);
  payloadBytes.insert(payloadBytes.end(), totalChrSize, 0x00);

  return std::make_shared<Cartridge>(header, std::move(payloadBytes));
}

TEST_CASE("CPU", "[cpu]") {
  auto cpuMemory = CPUMemory{};

  std::vector<u8> program = {0xea, 0xa9, 0x05, 0x8d, 0x01, 0x02};
  auto cartridge = createMockCartridge(program);

  auto cpu = std::make_shared<CPU>(cpuMemory);
  auto ppu = std::make_shared<PPU>(*cpu);
  auto apu = std::make_shared<APU>(cpu.get());
  auto mapper = std::make_shared<NROM>(cpu, ppu, cartridge);

  cpu->memory().onLoad(ppu, apu, mapper);

  SECTION("CPU initialization") {
    REQUIRE(cpu->m_cycle == 0);
    REQUIRE(cpu->m_extraCycles == 0);
  }

  SECTION("register bounding and wrapping") {
    REQUIRE(cpu->m_a.getContent() == 0);
    REQUIRE(cpu->m_x.getContent() == 0);
    REQUIRE(cpu->m_y.getContent() == 0);
    REQUIRE(cpu->m_sp.getContent() == 0);
    REQUIRE(cpu->m_pc.getContent() == 0);

    for (int i = 0; i < 256; i++) {
      cpu->m_a.setContent(static_cast<u8>(i));
      REQUIRE(cpu->m_a.getContent() == i);
    }

    for (int i = 0; i < 65536; i++) {
      cpu->m_pc.setContent(static_cast<u16>(i));
      REQUIRE(cpu->m_pc.getContent() == i);
    }

    for (int i = -300; i < 600; i++) {
      u8 expected_wrapped_8 = static_cast<u8>(i & 0xff);
      cpu->m_a.setContent(static_cast<u8>(i));
      REQUIRE(cpu->m_a.getContent() == expected_wrapped_8);
    }

    for (int i = -300; i < 65800; i++) {
      u16 expected_wrapped_16 = static_cast<u16>(i & 0xffff);
      cpu->m_pc.setContent(static_cast<u16>(i));
      REQUIRE(cpu->m_pc.getContent() == expected_wrapped_16);
    }
  }

  SECTION("flags register functionality") {
    REQUIRE(cpu->m_flags.m_c == false);
    REQUIRE(cpu->m_flags.m_z == false);
    REQUIRE(cpu->m_flags.m_i == false);
    REQUIRE(cpu->m_flags.m_d == false);
    REQUIRE(cpu->m_flags.m_v == false);
    REQUIRE(cpu->m_flags.m_n == false);

    cpu->m_flags.m_i = false;
    REQUIRE((cpu->m_flags.getValue() & 0b00100100) == 0b00000000);

    cpu->m_flags.m_z = true;
    REQUIRE((cpu->m_flags.getValue() & 0b00000010) == 0b00000010);

    cpu->m_flags.m_c = true;
    REQUIRE((cpu->m_flags.getValue() & 0b00000001) == 0b00000001);

    cpu->m_flags.setValue(0b11111111);
    REQUIRE(cpu->m_flags.m_c == true);
    REQUIRE(cpu->m_flags.m_z == true);
    REQUIRE(cpu->m_flags.m_i == true);
    REQUIRE(cpu->m_flags.m_d == true);
    REQUIRE(cpu->m_flags.m_v == true);
    REQUIRE(cpu->m_flags.m_n == true);

    cpu->m_flags.setValue(0b00000001);
    REQUIRE(cpu->m_flags.m_c == true);

    cpu->m_flags.setValue(0b00000010);
    REQUIRE(cpu->m_flags.m_z == true);

    cpu->m_flags.setValue(0b10000000);
    REQUIRE(cpu->m_flags.m_n == true);
  }

  SECTION("mutators and stuff") {
    u8 original_a = cpu->m_a.getContent();
    cpu->m_a.increment();
    cpu->m_a.increment();
    cpu->m_a.decrement();
    REQUIRE(cpu->m_a.getContent() == original_a + 1);

    cpu->m_flags.m_z = false;
    cpu->m_flags.updateZero(0);
    REQUIRE(cpu->m_flags.m_z == true);

    cpu->m_flags.updateNegative(129);
    REQUIRE(cpu->m_flags.m_n == true);

    cpu->m_flags.updateNegativeAndZero(0);
    REQUIRE(cpu->m_flags.m_z == true);
    REQUIRE(cpu->m_flags.m_n == false);
  }

  SECTION("stack and little-endian conversion") {
    cpu->m_sp.setContent(0xff);

    std::vector<u8> byte_stack_sequence = {0xaa, 0xbb, 0xcc, 0xdd};
    for (u8 val : byte_stack_sequence) {
      cpu->m_stack.push(val);
    }
    REQUIRE(cpu->m_stack.pop() == 0xdd);
    REQUIRE(cpu->m_stack.pop() == 0xcc);

    cpu->m_sp.setContent(0xff);
    cpu->m_stack.push16(0x1234);

    REQUIRE(cpu->m_stack.pop() == 0x34);
    REQUIRE(cpu->m_stack.pop() == 0x12);
  }

  SECTION("instruction lookup") {
    REQUIRE(OPERATIONS.size() == 256);

    cpu->m_pc.setContent(0x8000);
    Operation op_fetched = cpu->fetchOperation();
    REQUIRE(op_fetched.id == 0xea);
    REQUIRE(op_fetched.cycles == 2);
    REQUIRE(op_fetched.addressingMode->m_id == AddressingModeId::IMPLICIT);

    Operation op_lda = cpu->fetchOperation();
    REQUIRE(cpu->fetchInput(op_lda) == 0x05);

    Operation op_sta = cpu->fetchOperation();
    REQUIRE(cpu->fetchInput(op_sta) == 0x0201);

    cpu->m_x.setContent(0x06);
    u16 calculated_target_addr = _zpx.getAddress(*cpu, 0x40, false);
    REQUIRE(calculated_target_addr == 0x46);

    u8 immediate_resolved_value = _imm.getValue(*cpu, 0x05, false);
    REQUIRE(immediate_resolved_value == 0x05);

    cpu->m_cycle = 5;
    cpu->m_extraCycles = 3;
    Operation op_nop = OPERATIONS[0xea];
    size_t added_weight = cpu->addCycles(op_nop);

    REQUIRE(added_weight == 5);
    REQUIRE(cpu->m_cycle == 10);
    REQUIRE(cpu->m_extraCycles == 0);
  }

  SECTION("execution history logging") {
    auto historyPtr = get_m_executionHistory();
    std::vector<CPULogFrame> &history = (*cpu).*historyPtr;

    cpu->m_pc.setContent(0x8000);
    history.clear();

    cpu->step();
    REQUIRE(history.size() == 1);
    REQUIRE(history.back().pc == 0x8000);
    REQUIRE(history.back().opcode == 0xea);

    cpu->step();
    REQUIRE(history.size() == 2);
    REQUIRE(history.back().pc == 0x8001);
    REQUIRE(history.back().opcode == 0xa9);
  }
}

TEST_CASE("CPU Timing Pipeline - Demo Synchronization Verification",
          "[cpu][timing]") {

  SECTION("Verify Disabled Decimal Mode on Math Steps") {
    CPUMemory cpuMemory;
    CPU cpu(cpuMemory);

    cpu.m_flags.m_d = true;
    cpu.m_a.setContent(0x09);

    Instructions::ADC.runValue(cpu, 0x01);

    REQUIRE(cpu.m_a.getContent() == 0x0a);
  }

  SECTION("Branch Instruction Page Crossing Cycle Penalty Verification") {
    CPUMemory cpuMemory;
    CPU cpu(cpuMemory);

    cpu.m_pc.setContent(0x80ff);
    cpu.m_cycle = 0;
    cpu.m_extraCycles = 0;
    cpu.m_flags.m_z = true;

    auto operation = OPERATIONS[0xf0];
    u16 offsetArgument = 0x05;

    u16 targetAddress =
        _rel.getAddress(cpu, offsetArgument, operation.hasPageCrossPenalty);

    operation.instruction.runAddress(cpu, targetAddress);

    u8 totalCyclesConsumed = operation.cycles + cpu.m_extraCycles;

    REQUIRE(totalCyclesConsumed == 4);

    REQUIRE(cpu.m_pc.getContent() == 0x8104);
  }
  SECTION("Branch Instruction Page Crossing - Comprehensive Validation") {
    CPUMemory cpuMemory;
    CPU cpu(cpuMemory);
    auto operation = OPERATIONS[0xf0]; // BEQ

    cpu.m_pc.setContent(0x80ff);
    cpu.m_extraCycles = 0;
    cpu.m_flags.m_z = true;

    u16 targetForward =
        _rel.getAddress(cpu, 0x05, operation.hasPageCrossPenalty);
    REQUIRE(targetForward == 0x8104);

    operation.instruction.runAddress(cpu, targetForward);
    REQUIRE(cpu.m_extraCycles == 2);

    cpu.m_pc.setContent(0x8101);
    cpu.m_extraCycles = 0;
    cpu.m_flags.m_z = true;

    u16 targetBackward =
        _rel.getAddress(cpu, 0xfb, operation.hasPageCrossPenalty);
    REQUIRE(targetBackward == 0x80fc);

    operation.instruction.runAddress(cpu, targetBackward);
  }
}

TEST_CASE("CPU Instruction Core - Physics Loop Sync Alignment Matrix",
          "[cpu][physics]") {
  CPUMemory cpuMemory;
  CPU cpu(cpuMemory);

  SECTION(
      "SBC Hardware-Accurate Inverted Borrow and Carry Flag Update Matrix") {
    cpu.m_a.setContent(0x00);
    cpu.m_flags.m_c = false;

    Instructions::SBC.runValue(cpu, 0x01);

    REQUIRE(cpu.m_a.getContent() == 0xfe);

    REQUIRE(cpu.m_flags.m_c == false);
  }

  SECTION("CMP Strict Negative Flag Bit-7 Extraction Rule") {
    cpu.m_a.setContent(0x00);

    Instructions::CMP.runValue(cpu, 0x80);

    REQUIRE(cpu.m_flags.m_n == true);
    REQUIRE(cpu.m_flags.m_c == false);
  }
}
