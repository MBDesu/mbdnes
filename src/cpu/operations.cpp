#include "cpu/operations.h"

const std::array<Operation, 256> RAW_LIST = {
    // ----------------- ADC -----------------
    Operation{
        .id = 0x69,
        .instruction = Instructions::ADC,
        .cycles = 2,
        .addressingMode =
            ADDRESSING_MODES[static_cast<size_t>(AddressingModeId::IMMEDIATE)]},
    Operation{
        .id = 0x65,
        .instruction = Instructions::ADC,
        .cycles = 3,
        .addressingMode =
            ADDRESSING_MODES[static_cast<size_t>(AddressingModeId::ZERO_PAGE)]},
    Operation{.id = 0x75,
              .instruction = Instructions::ADC,
              .cycles = 4,
              .addressingMode = ADDRESSING_MODES[static_cast<size_t>(
                  AddressingModeId::INDEXED_ZERO_PAGE_X)]},
    Operation{
        .id = 0x6d,
        .instruction = Instructions::ADC,
        .cycles = 4,
        .addressingMode =
            ADDRESSING_MODES[static_cast<size_t>(AddressingModeId::ABSOLUTE)]},
    Operation{.id = 0x7d,
              .instruction = Instructions::ADC,
              .cycles = 4,
              .addressingMode = ADDRESSING_MODES[static_cast<size_t>(
                  AddressingModeId::INDEXED_ABSOLUTE_X)],
              .hasPageCrossPenalty = true},
    Operation{.id = 0x79,
              .instruction = Instructions::ADC,
              .cycles = 4,
              .addressingMode = ADDRESSING_MODES[static_cast<size_t>(
                  AddressingModeId::INDEXED_ABSOLUTE_Y)],
              .hasPageCrossPenalty = true},
    Operation{.id = 0x61,
              .instruction = Instructions::ADC,
              .cycles = 6,
              .addressingMode = ADDRESSING_MODES[static_cast<size_t>(
                  AddressingModeId::INDEXED_INDIRECT)]},
    Operation{.id = 0x71,
              .instruction = Instructions::ADC,
              .cycles = 5,
              .addressingMode = ADDRESSING_MODES[static_cast<size_t>(
                  AddressingModeId::INDIRECT_INDEXED)],
              .hasPageCrossPenalty = true},

    // ----------------- AND -----------------
    Operation{
        .id = 0x29,
        .instruction = Instructions::AND,
        .cycles = 2,
        .addressingMode =
            ADDRESSING_MODES[static_cast<size_t>(AddressingModeId::IMMEDIATE)]},
    Operation{
        .id = 0x25,
        .instruction = Instructions::AND,
        .cycles = 3,
        .addressingMode =
            ADDRESSING_MODES[static_cast<size_t>(AddressingModeId::ZERO_PAGE)]},
    Operation{.id = 0x35,
              .instruction = Instructions::AND,
              .cycles = 4,
              .addressingMode = ADDRESSING_MODES[static_cast<size_t>(
                  AddressingModeId::INDEXED_ZERO_PAGE_X)]},
    Operation{
        .id = 0x2d,
        .instruction = Instructions::AND,
        .cycles = 4,
        .addressingMode =
            ADDRESSING_MODES[static_cast<size_t>(AddressingModeId::ABSOLUTE)]},
    Operation{.id = 0x3d,
              .instruction = Instructions::AND,
              .cycles = 4,
              .addressingMode = ADDRESSING_MODES[static_cast<size_t>(
                  AddressingModeId::INDEXED_ABSOLUTE_X)],
              .hasPageCrossPenalty = true},
    Operation{.id = 0x39,
              .instruction = Instructions::AND,
              .cycles = 4,
              .addressingMode = ADDRESSING_MODES[static_cast<size_t>(
                  AddressingModeId::INDEXED_ABSOLUTE_Y)],
              .hasPageCrossPenalty = true},
    Operation{.id = 0x21,
              .instruction = Instructions::AND,
              .cycles = 6,
              .addressingMode = ADDRESSING_MODES[static_cast<size_t>(
                  AddressingModeId::INDEXED_INDIRECT)]},
    Operation{.id = 0x31,
              .instruction = Instructions::AND,
              .cycles = 5,
              .addressingMode = ADDRESSING_MODES[static_cast<size_t>(
                  AddressingModeId::INDIRECT_INDEXED)],
              .hasPageCrossPenalty = true},

    // ----------------- ASL -----------------
    Operation{
        .id = 0x0a,
        .instruction = Instructions::ASLa,
        .cycles = 2,
        .addressingMode =
            ADDRESSING_MODES[static_cast<size_t>(AddressingModeId::IMPLICIT)]},
    Operation{
        .id = 0x06,
        .instruction = Instructions::ASL,
        .cycles = 5,
        .addressingMode =
            ADDRESSING_MODES[static_cast<size_t>(AddressingModeId::ZERO_PAGE)]},
    Operation{.id = 0x16,
              .instruction = Instructions::ASL,
              .cycles = 6,
              .addressingMode = ADDRESSING_MODES[static_cast<size_t>(
                  AddressingModeId::INDEXED_ZERO_PAGE_X)]},
    Operation{
        .id = 0x0e,
        .instruction = Instructions::ASL,
        .cycles = 6,
        .addressingMode =
            ADDRESSING_MODES[static_cast<size_t>(AddressingModeId::ABSOLUTE)]},
    Operation{.id = 0x1e,
              .instruction = Instructions::ASL,
              .cycles = 7,
              .addressingMode = ADDRESSING_MODES[static_cast<size_t>(
                  AddressingModeId::INDEXED_ABSOLUTE_X)]},

    // ----------------- Branches & Jumps -----------------
    Operation{
        .id = 0x90,
        .instruction = Instructions::BCC,
        .cycles = 2,
        .addressingMode =
            ADDRESSING_MODES[static_cast<size_t>(AddressingModeId::RELATIVE)],
        .hasPageCrossPenalty = false},
    Operation{
        .id = 0xb0,
        .instruction = Instructions::BCS,
        .cycles =
            2,
        .addressingMode =
            ADDRESSING_MODES[static_cast<size_t>(AddressingModeId::RELATIVE)],
        .hasPageCrossPenalty = false},
    Operation{
        .id = 0xf0,
        .instruction = Instructions::BEQ,
        .cycles = 2,
        .addressingMode =
            ADDRESSING_MODES[static_cast<size_t>(AddressingModeId::RELATIVE)],
        .hasPageCrossPenalty = false,
    },
    Operation{
        .id = 0x30,
        .instruction = Instructions::BMI,
        .cycles = 2,
        .addressingMode =
            ADDRESSING_MODES[static_cast<size_t>(AddressingModeId::RELATIVE)],
        .hasPageCrossPenalty = false,
    },
    Operation{
        .id = 0xd0,
        .instruction = Instructions::BNE,
        .cycles = 2,
        .addressingMode =
            ADDRESSING_MODES[static_cast<size_t>(AddressingModeId::RELATIVE)],
        .hasPageCrossPenalty = false,
    },
    Operation{
        .id = 0x10,
        .instruction = Instructions::BPL,
        .cycles = 2,
        .addressingMode =
            ADDRESSING_MODES[static_cast<size_t>(AddressingModeId::RELATIVE)],
        .hasPageCrossPenalty = false,
    },
    Operation{
        .id = 0x50,
        .instruction = Instructions::BVC,
        .cycles = 2,
        .addressingMode =
            ADDRESSING_MODES[static_cast<size_t>(AddressingModeId::RELATIVE)],
        .hasPageCrossPenalty = false,
    },
    Operation{
        .id = 0x70,
        .instruction = Instructions::BVS,
        .cycles = 2,
        .addressingMode =
            ADDRESSING_MODES[static_cast<size_t>(AddressingModeId::RELATIVE)],
        .hasPageCrossPenalty = false,
    },
    Operation{
        .id = 0x4c,
        .instruction = Instructions::JMP,
        .cycles =
            3,
        .addressingMode =
            ADDRESSING_MODES[static_cast<size_t>(AddressingModeId::ABSOLUTE)]},
    Operation{
        .id = 0x6c,
        .instruction = Instructions::JMP,
        .cycles =
            5,
        .addressingMode =
            ADDRESSING_MODES[static_cast<size_t>(AddressingModeId::INDIRECT)]},
    Operation{
        .id = 0x20,
        .instruction = Instructions::JSR,
        .cycles =
            6,
        .addressingMode =
            ADDRESSING_MODES[static_cast<size_t>(AddressingModeId::ABSOLUTE)]},

    // ----------------- Bit Tests & Breaks -----------------
    Operation{
        .id = 0x24,
        .instruction = Instructions::BIT,
        .cycles =
            3,
        .addressingMode =
            ADDRESSING_MODES[static_cast<size_t>(AddressingModeId::ZERO_PAGE)]},
    Operation{
        .id = 0x2c,
        .instruction = Instructions::BIT,
        .cycles =
            4,
        .addressingMode =
            ADDRESSING_MODES[static_cast<size_t>(AddressingModeId::ABSOLUTE)]},
    Operation{
        .id = 0x00,
        .instruction = Instructions::BRK,
        .cycles =
            7,
        .addressingMode =
            ADDRESSING_MODES[static_cast<size_t>(AddressingModeId::IMPLICIT)]},

    // ----------------- Clears & Sets (Flags) -----------------
    Operation{
        .id = 0x18,
        .instruction = Instructions::CLC,
        .cycles =
            2,
        .addressingMode =
            ADDRESSING_MODES[static_cast<size_t>(AddressingModeId::IMPLICIT)]},
    Operation{
        .id = 0xd8,
        .instruction = Instructions::CLD,
        .cycles =
            2,
        .addressingMode =
            ADDRESSING_MODES[static_cast<size_t>(AddressingModeId::IMPLICIT)]},
    Operation{
        .id = 0x58,
        .instruction = Instructions::CLI,
        .cycles =
            2,
        .addressingMode =
            ADDRESSING_MODES[static_cast<size_t>(AddressingModeId::IMPLICIT)]},
    Operation{
        .id = 0xb8,
        .instruction = Instructions::CLV,
        .cycles =
            2,
        .addressingMode =
            ADDRESSING_MODES[static_cast<size_t>(AddressingModeId::IMPLICIT)]},
    Operation{
        .id = 0x38,
        .instruction = Instructions::SEC,
        .cycles =
            2,
        .addressingMode =
            ADDRESSING_MODES[static_cast<size_t>(AddressingModeId::IMPLICIT)]},
    Operation{
        .id = 0xf8,
        .instruction = Instructions::SED,
        .cycles =
            2,
        .addressingMode =
            ADDRESSING_MODES[static_cast<size_t>(AddressingModeId::IMPLICIT)]},
    Operation{
        .id = 0x78,
        .instruction = Instructions::SEI,
        .cycles =
            2,
        .addressingMode =
            ADDRESSING_MODES[static_cast<size_t>(AddressingModeId::IMPLICIT)]},

    // ----------------- CMP -----------------
    Operation{
        .id = 0xc9,
        .instruction = Instructions::CMP,
        .cycles =
            2,
        .addressingMode =
            ADDRESSING_MODES[static_cast<size_t>(AddressingModeId::IMMEDIATE)]},
    Operation{
        .id = 0xc5,
        .instruction = Instructions::CMP,
        .cycles =
            3,
        .addressingMode =
            ADDRESSING_MODES[static_cast<size_t>(AddressingModeId::ZERO_PAGE)]},
    Operation{.id = 0xd5,
              .instruction = Instructions::CMP,
              .cycles = 4,
              .addressingMode = ADDRESSING_MODES[static_cast<size_t>(
                  AddressingModeId::INDEXED_ZERO_PAGE_X)]},
    Operation{
        .id = 0xcd,
        .instruction = Instructions::CMP,
        .cycles =
            4,
        .addressingMode =
            ADDRESSING_MODES[static_cast<size_t>(AddressingModeId::ABSOLUTE)]},
    Operation{.id = 0xdd,
              .instruction = Instructions::CMP,
              .cycles = 4,
              .addressingMode = ADDRESSING_MODES[static_cast<size_t>(
                  AddressingModeId::INDEXED_ABSOLUTE_X)],
              .hasPageCrossPenalty = true},
    Operation{.id = 0xd9,
              .instruction = Instructions::CMP,
              .cycles = 4,
              .addressingMode = ADDRESSING_MODES[static_cast<size_t>(
                  AddressingModeId::INDEXED_ABSOLUTE_Y)],
              .hasPageCrossPenalty = true},
    Operation{.id = 0xc1,
              .instruction = Instructions::CMP,
              .cycles = 6,
              .addressingMode = ADDRESSING_MODES[static_cast<size_t>(
                  AddressingModeId::INDEXED_INDIRECT)]},
    Operation{.id = 0xd1,
              .instruction = Instructions::CMP,
              .cycles = 5,
              .addressingMode = ADDRESSING_MODES[static_cast<size_t>(
                  AddressingModeId::INDIRECT_INDEXED)],
              .hasPageCrossPenalty = true},

    // ----------------- CPX & CPY -----------------
    Operation{
        .id = 0xe0,
        .instruction = Instructions::CPX,
        .cycles =
            2,
        .addressingMode =
            ADDRESSING_MODES[static_cast<size_t>(AddressingModeId::IMMEDIATE)]},
    Operation{
        .id = 0xe4,
        .instruction = Instructions::CPX,
        .cycles =
            3,
        .addressingMode =
            ADDRESSING_MODES[static_cast<size_t>(AddressingModeId::ZERO_PAGE)]},
    Operation{
        .id = 0xec,
        .instruction = Instructions::CPX,
        .cycles =
            4,
        .addressingMode =
            ADDRESSING_MODES[static_cast<size_t>(AddressingModeId::ABSOLUTE)]},
    Operation{
        .id = 0xc0,
        .instruction = Instructions::CPY,
        .cycles =
            2,
        .addressingMode =
            ADDRESSING_MODES[static_cast<size_t>(AddressingModeId::IMMEDIATE)]},
    Operation{
        .id = 0xc4,
        .instruction = Instructions::CPY,
        .cycles =
            3,
        .addressingMode =
            ADDRESSING_MODES[static_cast<size_t>(AddressingModeId::ZERO_PAGE)]},
    Operation{
        .id = 0xcc,
        .instruction = Instructions::CPY,
        .cycles =
            4,
        .addressingMode =
            ADDRESSING_MODES[static_cast<size_t>(AddressingModeId::ABSOLUTE)]},

    // ----------------- Decrements & Increments -----------------
    Operation{
        .id = 0xc6,
        .instruction = Instructions::DEC,
        .cycles =
            5,
        .addressingMode =
            ADDRESSING_MODES[static_cast<size_t>(AddressingModeId::ZERO_PAGE)]},
    Operation{.id = 0xd6,
              .instruction = Instructions::DEC,
              .cycles = 6,
              .addressingMode = ADDRESSING_MODES[static_cast<size_t>(
                  AddressingModeId::INDEXED_ZERO_PAGE_X)]},
    Operation{
        .id = 0xce,
        .instruction = Instructions::DEC,
        .cycles =
            6,
        .addressingMode =
            ADDRESSING_MODES[static_cast<size_t>(AddressingModeId::ABSOLUTE)]},
    Operation{.id = 0xde,
              .instruction = Instructions::DEC,
              .cycles = 7,
              .addressingMode = ADDRESSING_MODES[static_cast<size_t>(
                  AddressingModeId::INDEXED_ABSOLUTE_X)]},
    Operation{
        .id = 0xca,
        .instruction = Instructions::DEX,
        .cycles =
            2,
        .addressingMode =
            ADDRESSING_MODES[static_cast<size_t>(AddressingModeId::IMPLICIT)]},
    Operation{
        .id = 0x88,
        .instruction = Instructions::DEY,
        .cycles =
            2,
        .addressingMode =
            ADDRESSING_MODES[static_cast<size_t>(AddressingModeId::IMPLICIT)]},
    Operation{
        .id = 0xe6,
        .instruction = Instructions::INC,
        .cycles =
            5,
        .addressingMode =
            ADDRESSING_MODES[static_cast<size_t>(AddressingModeId::ZERO_PAGE)]},
    Operation{.id = 0xf6,
              .instruction = Instructions::INC,
              .cycles = 6,
              .addressingMode = ADDRESSING_MODES[static_cast<size_t>(
                  AddressingModeId::INDEXED_ZERO_PAGE_X)]},
    Operation{
        .id = 0xee,
        .instruction = Instructions::INC,
        .cycles =
            6,
        .addressingMode =
            ADDRESSING_MODES[static_cast<size_t>(AddressingModeId::ABSOLUTE)]},
    Operation{.id = 0xfe,
              .instruction = Instructions::INC,
              .cycles = 7,
              .addressingMode = ADDRESSING_MODES[static_cast<size_t>(
                  AddressingModeId::INDEXED_ABSOLUTE_X)]},
    Operation{
        .id = 0xe8,
        .instruction = Instructions::INX,
        .cycles =
            2,
        .addressingMode =
            ADDRESSING_MODES[static_cast<size_t>(AddressingModeId::IMPLICIT)]},
    Operation{
        .id = 0xc8,
        .instruction = Instructions::INY,
        .cycles =
            2,
        .addressingMode =
            ADDRESSING_MODES[static_cast<size_t>(AddressingModeId::IMPLICIT)]},
    // ----------------- EOR -----------------
    Operation{
        .id = 0x49,
        .instruction = Instructions::EOR,
        .cycles =
            2,
        .addressingMode =
            ADDRESSING_MODES[static_cast<size_t>(AddressingModeId::IMMEDIATE)]},
    Operation{
        .id = 0x45,
        .instruction = Instructions::EOR,
        .cycles =
            3,
        .addressingMode =
            ADDRESSING_MODES[static_cast<size_t>(AddressingModeId::ZERO_PAGE)]},
    Operation{.id = 0x55,
              .instruction = Instructions::EOR,
              .cycles = 4,
              .addressingMode = ADDRESSING_MODES[static_cast<size_t>(
                  AddressingModeId::INDEXED_ZERO_PAGE_X)]},
    Operation{
        .id = 0x4d,
        .instruction = Instructions::EOR,
        .cycles =
            4,
        .addressingMode =
            ADDRESSING_MODES[static_cast<size_t>(AddressingModeId::ABSOLUTE)]},
    Operation{.id = 0x5d,
              .instruction = Instructions::EOR,
              .cycles = 4,
              .addressingMode = ADDRESSING_MODES[static_cast<size_t>(
                  AddressingModeId::INDEXED_ABSOLUTE_X)],
              .hasPageCrossPenalty = true},
    Operation{.id = 0x59,
              .instruction = Instructions::EOR,
              .cycles = 4,
              .addressingMode = ADDRESSING_MODES[static_cast<size_t>(
                  AddressingModeId::INDEXED_ABSOLUTE_Y)],
              .hasPageCrossPenalty = true},
    Operation{.id = 0x41,
              .instruction = Instructions::EOR,
              .cycles = 6,
              .addressingMode = ADDRESSING_MODES[static_cast<size_t>(
                  AddressingModeId::INDEXED_INDIRECT)]},
    Operation{.id = 0x51,
              .instruction = Instructions::EOR,
              .cycles = 5,
              .addressingMode = ADDRESSING_MODES[static_cast<size_t>(
                  AddressingModeId::INDIRECT_INDEXED)],
              .hasPageCrossPenalty = true},
    // ----------------- LDA -----------------
    Operation{
        .id = 0xa9,
        .instruction = Instructions::LDA,
        .cycles =
            2,
        .addressingMode =
            ADDRESSING_MODES[static_cast<size_t>(AddressingModeId::IMMEDIATE)]},
    Operation{
        .id = 0xa5,
        .instruction = Instructions::LDA,
        .cycles =
            3,
        .addressingMode =
            ADDRESSING_MODES[static_cast<size_t>(AddressingModeId::ZERO_PAGE)]},
    Operation{.id = 0xb5,
              .instruction = Instructions::LDA,
              .cycles = 4,
              .addressingMode = ADDRESSING_MODES[static_cast<size_t>(
                  AddressingModeId::INDEXED_ZERO_PAGE_X)]},
    Operation{
        .id = 0xad,
        .instruction = Instructions::LDA,
        .cycles =
            4,
        .addressingMode =
            ADDRESSING_MODES[static_cast<size_t>(AddressingModeId::ABSOLUTE)]},
    Operation{.id = 0xbd,
              .instruction = Instructions::LDA,
              .cycles = 4,
              .addressingMode = ADDRESSING_MODES[static_cast<size_t>(
                  AddressingModeId::INDEXED_ABSOLUTE_X)],
              .hasPageCrossPenalty = true},
    Operation{.id = 0xb9,
              .instruction = Instructions::LDA,
              .cycles = 4,
              .addressingMode = ADDRESSING_MODES[static_cast<size_t>(
                  AddressingModeId::INDEXED_ABSOLUTE_Y)],
              .hasPageCrossPenalty = true},
    Operation{.id = 0xa1,
              .instruction = Instructions::LDA,
              .cycles = 6,
              .addressingMode = ADDRESSING_MODES[static_cast<size_t>(
                  AddressingModeId::INDEXED_INDIRECT)]},
    Operation{.id = 0xb1,
              .instruction = Instructions::LDA,
              .cycles = 5,
              .addressingMode = ADDRESSING_MODES[static_cast<size_t>(
                  AddressingModeId::INDIRECT_INDEXED)],
              .hasPageCrossPenalty = true},
    // ----------------- LDX & LDY -----------------
    Operation{
        .id = 0xa2,
        .instruction = Instructions::LDX,
        .cycles =
            2,
        .addressingMode =
            ADDRESSING_MODES[static_cast<size_t>(AddressingModeId::IMMEDIATE)]},
    Operation{
        .id = 0xa6,
        .instruction = Instructions::LDX,
        .cycles =
            3,
        .addressingMode =
            ADDRESSING_MODES[static_cast<size_t>(AddressingModeId::ZERO_PAGE)]},
    Operation{.id = 0xb6,
              .instruction = Instructions::LDX,
              .cycles = 4,
              .addressingMode = ADDRESSING_MODES[static_cast<size_t>(
                  AddressingModeId::INDEXED_ZERO_PAGE_Y)]},
    Operation{
        .id = 0xae,
        .instruction = Instructions::LDX,
        .cycles =
            4,
        .addressingMode =
            ADDRESSING_MODES[static_cast<size_t>(AddressingModeId::ABSOLUTE)]},
    Operation{.id = 0xbe,
              .instruction = Instructions::LDX,
              .cycles = 4,
              .addressingMode = ADDRESSING_MODES[static_cast<size_t>(
                  AddressingModeId::INDEXED_ABSOLUTE_Y)],
              .hasPageCrossPenalty = true},
    Operation{
        .id = 0xa0,
        .instruction = Instructions::LDY,
        .cycles =
            2,
        .addressingMode =
            ADDRESSING_MODES[static_cast<size_t>(AddressingModeId::IMMEDIATE)]},
    Operation{
        .id = 0xa4,
        .instruction = Instructions::LDY,
        .cycles =
            3,
        .addressingMode =
            ADDRESSING_MODES[static_cast<size_t>(AddressingModeId::ZERO_PAGE)]},
    Operation{.id = 0xb4,
              .instruction = Instructions::LDY,
              .cycles = 4,
              .addressingMode = ADDRESSING_MODES[static_cast<size_t>(
                  AddressingModeId::INDEXED_ZERO_PAGE_X)]},
    Operation{
        .id = 0xac,
        .instruction = Instructions::LDY,
        .cycles =
            4,
        .addressingMode =
            ADDRESSING_MODES[static_cast<size_t>(AddressingModeId::ABSOLUTE)]},
    Operation{.id = 0xbc,
              .instruction = Instructions::LDY,
              .cycles = 4,
              .addressingMode = ADDRESSING_MODES[static_cast<size_t>(
                  AddressingModeId::INDEXED_ABSOLUTE_X)],
              .hasPageCrossPenalty = true},
    // ----------------- LSR -----------------
    Operation{
        .id = 0x4a,
        .instruction = Instructions::LSRa,
        .cycles =
            2,
        .addressingMode =
            ADDRESSING_MODES[static_cast<size_t>(AddressingModeId::IMPLICIT)]},
    Operation{
        .id = 0x46,
        .instruction = Instructions::LSR,
        .cycles =
            5,
        .addressingMode =
            ADDRESSING_MODES[static_cast<size_t>(AddressingModeId::ZERO_PAGE)]},
    Operation{.id = 0x56,
              .instruction = Instructions::LSR,
              .cycles = 6,
              .addressingMode = ADDRESSING_MODES[static_cast<size_t>(
                  AddressingModeId::INDEXED_ZERO_PAGE_X)]},
    Operation{
        .id = 0x4e,
        .instruction = Instructions::LSR,
        .cycles =
            6,
        .addressingMode =
            ADDRESSING_MODES[static_cast<size_t>(AddressingModeId::ABSOLUTE)]},
    Operation{.id = 0x5e,
              .instruction = Instructions::LSR,
              .cycles = 7,
              .addressingMode = ADDRESSING_MODES[static_cast<size_t>(
                  AddressingModeId::INDEXED_ABSOLUTE_X)]},
    // ----------------- ORA -----------------
    Operation{
        .id = 0x09,
        .instruction = Instructions::ORA,
        .cycles =
            2,
        .addressingMode =
            ADDRESSING_MODES[static_cast<size_t>(AddressingModeId::IMMEDIATE)]},
    Operation{
        .id = 0x05,
        .instruction = Instructions::ORA,
        .cycles =
            3,
        .addressingMode =
            ADDRESSING_MODES[static_cast<size_t>(AddressingModeId::ZERO_PAGE)]},
    Operation{.id = 0x15,
              .instruction = Instructions::ORA,
              .cycles = 4,
              .addressingMode = ADDRESSING_MODES[static_cast<size_t>(
                  AddressingModeId::INDEXED_ZERO_PAGE_X)]},
    Operation{
        .id = 0x0d,
        .instruction = Instructions::ORA,
        .cycles =
            4,
        .addressingMode =
            ADDRESSING_MODES[static_cast<size_t>(AddressingModeId::ABSOLUTE)]},
    Operation{.id = 0x1d,
              .instruction = Instructions::ORA,
              .cycles = 4,
              .addressingMode = ADDRESSING_MODES[static_cast<size_t>(
                  AddressingModeId::INDEXED_ABSOLUTE_X)],
              .hasPageCrossPenalty = true},
    Operation{.id = 0x19,
              .instruction = Instructions::ORA,
              .cycles = 4,
              .addressingMode = ADDRESSING_MODES[static_cast<size_t>(
                  AddressingModeId::INDEXED_ABSOLUTE_Y)],
              .hasPageCrossPenalty = true},
    Operation{.id = 0x01,
              .instruction = Instructions::ORA,
              .cycles = 6,
              .addressingMode = ADDRESSING_MODES[static_cast<size_t>(
                  AddressingModeId::INDEXED_INDIRECT)]},
    Operation{.id = 0x11,
              .instruction = Instructions::ORA,
              .cycles = 5,
              .addressingMode = ADDRESSING_MODES[static_cast<size_t>(
                  AddressingModeId::INDIRECT_INDEXED)],
              .hasPageCrossPenalty = true},
    // ----------------- Stack & NOP -----------------
    Operation{
        .id = 0x48,
        .instruction = Instructions::PHA,
        .cycles =
            3,
        .addressingMode =
            ADDRESSING_MODES[static_cast<size_t>(AddressingModeId::IMPLICIT)]},
    Operation{
        .id = 0x08,
        .instruction = Instructions::PHP,
        .cycles =
            3,
        .addressingMode =
            ADDRESSING_MODES[static_cast<size_t>(AddressingModeId::IMPLICIT)]},
    Operation{
        .id = 0x68,
        .instruction = Instructions::PLA,
        .cycles =
            4,
        .addressingMode =
            ADDRESSING_MODES[static_cast<size_t>(AddressingModeId::IMPLICIT)]},
    Operation{
        .id = 0x28,
        .instruction = Instructions::PLP,
        .cycles =
            4,
        .addressingMode =
            ADDRESSING_MODES[static_cast<size_t>(AddressingModeId::IMPLICIT)]},
    Operation{
        .id = 0xea,
        .instruction = Instructions::NOP,
        .cycles =
            2,
        .addressingMode =
            ADDRESSING_MODES[static_cast<size_t>(AddressingModeId::IMPLICIT)]},
    // ----------------- ROL & ROR -----------------
    Operation{
        .id = 0x2a,
        .instruction = Instructions::ROLa,
        .cycles =
            2,
        .addressingMode =
            ADDRESSING_MODES[static_cast<size_t>(AddressingModeId::IMPLICIT)]},
    Operation{
        .id = 0x26,
        .instruction = Instructions::ROL,
        .cycles =
            5,
        .addressingMode =
            ADDRESSING_MODES[static_cast<size_t>(AddressingModeId::ZERO_PAGE)]},
    Operation{.id = 0x36,
              .instruction = Instructions::ROL,
              .cycles = 6,
              .addressingMode = ADDRESSING_MODES[static_cast<size_t>(
                  AddressingModeId::INDEXED_ZERO_PAGE_X)]},
    Operation{
        .id = 0x2e,
        .instruction = Instructions::ROL,
        .cycles =
            6,
        .addressingMode =
            ADDRESSING_MODES[static_cast<size_t>(AddressingModeId::ABSOLUTE)]},
    Operation{.id = 0x3e,
              .instruction = Instructions::ROL,
              .cycles = 7,
              .addressingMode = ADDRESSING_MODES[static_cast<size_t>(
                  AddressingModeId::INDEXED_ABSOLUTE_X)]},
    Operation{
        .id = 0x6a,
        .instruction = Instructions::RORa,
        .cycles =
            2,
        .addressingMode =
            ADDRESSING_MODES[static_cast<size_t>(AddressingModeId::IMPLICIT)]},
    Operation{
        .id = 0x66,
        .instruction = Instructions::ROR,
        .cycles =
            5,
        .addressingMode =
            ADDRESSING_MODES[static_cast<size_t>(AddressingModeId::ZERO_PAGE)]},
    Operation{.id = 0x76,
              .instruction = Instructions::ROR,
              .cycles = 6,
              .addressingMode = ADDRESSING_MODES[static_cast<size_t>(
                  AddressingModeId::INDEXED_ZERO_PAGE_X)]},
    Operation{
        .id = 0x6e,
        .instruction = Instructions::ROR,
        .cycles =
            6,
        .addressingMode =
            ADDRESSING_MODES[static_cast<size_t>(AddressingModeId::ABSOLUTE)]},
    Operation{.id = 0x7e,
              .instruction = Instructions::ROR,
              .cycles = 7,
              .addressingMode = ADDRESSING_MODES[static_cast<size_t>(
                  AddressingModeId::INDEXED_ABSOLUTE_X)]},
    // ----------------- Returns -----------------
    Operation{
        .id = 0x40,
        .instruction = Instructions::RTI,
        .cycles =
            6,
        .addressingMode =
            ADDRESSING_MODES[static_cast<size_t>(AddressingModeId::IMPLICIT)]},
    Operation{
        .id = 0x60,
        .instruction = Instructions::RTS,
        .cycles =
            6,
        .addressingMode =
            ADDRESSING_MODES[static_cast<size_t>(AddressingModeId::IMPLICIT)]},
    // ----------------- SBC -----------------
    Operation{
        .id = 0xe9,
        .instruction = Instructions::SBC,
        .cycles =
            2,
        .addressingMode =
            ADDRESSING_MODES[static_cast<size_t>(AddressingModeId::IMMEDIATE)]},
    Operation{
        .id = 0xe5,
        .instruction = Instructions::SBC,
        .cycles =
            3,
        .addressingMode =
            ADDRESSING_MODES[static_cast<size_t>(AddressingModeId::ZERO_PAGE)]},
    Operation{.id = 0xf5,
              .instruction = Instructions::SBC,
              .cycles = 4,
              .addressingMode = ADDRESSING_MODES[static_cast<size_t>(
                  AddressingModeId::INDEXED_ZERO_PAGE_X)]},
    Operation{
        .id = 0xed,
        .instruction = Instructions::SBC,
        .cycles =
            4,
        .addressingMode =
            ADDRESSING_MODES[static_cast<size_t>(AddressingModeId::ABSOLUTE)]},
    Operation{.id = 0xfd,
              .instruction = Instructions::SBC,
              .cycles = 4,
              .addressingMode = ADDRESSING_MODES[static_cast<size_t>(
                  AddressingModeId::INDEXED_ABSOLUTE_X)],
              .hasPageCrossPenalty = true},
    Operation{.id = 0xf9,
              .instruction = Instructions::SBC,
              .cycles = 4,
              .addressingMode = ADDRESSING_MODES[static_cast<size_t>(
                  AddressingModeId::INDEXED_ABSOLUTE_Y)],
              .hasPageCrossPenalty = true},
    Operation{.id = 0xe1,
              .instruction = Instructions::SBC,
              .cycles = 6,
              .addressingMode = ADDRESSING_MODES[static_cast<size_t>(
                  AddressingModeId::INDEXED_INDIRECT)]},
    Operation{.id = 0xf1,
              .instruction = Instructions::SBC,
              .cycles = 5,
              .addressingMode = ADDRESSING_MODES[static_cast<size_t>(
                  AddressingModeId::INDIRECT_INDEXED)],
              .hasPageCrossPenalty = true},
    // ----------------- STA, STX, STY -----------------
    Operation{
        .id = 0x85,
        .instruction = Instructions::STA,
        .cycles =
            3,
        .addressingMode =
            ADDRESSING_MODES[static_cast<size_t>(AddressingModeId::ZERO_PAGE)]},
    Operation{.id = 0x95,
              .instruction = Instructions::STA,
              .cycles = 4,
              .addressingMode = ADDRESSING_MODES[static_cast<size_t>(
                  AddressingModeId::INDEXED_ZERO_PAGE_X)]},
    Operation{
        .id = 0x8d,
        .instruction = Instructions::STA,
        .cycles =
            4,
        .addressingMode =
            ADDRESSING_MODES[static_cast<size_t>(AddressingModeId::ABSOLUTE)]},
    Operation{.id = 0x9d,
              .instruction = Instructions::STA,
              .cycles = 5,
              .addressingMode = ADDRESSING_MODES[static_cast<size_t>(
                  AddressingModeId::INDEXED_ABSOLUTE_X)]},
    Operation{.id = 0x99,
              .instruction = Instructions::STA,
              .cycles = 5,
              .addressingMode = ADDRESSING_MODES[static_cast<size_t>(
                  AddressingModeId::INDEXED_ABSOLUTE_Y)]},
    Operation{.id = 0x81,
              .instruction = Instructions::STA,
              .cycles = 6,
              .addressingMode = ADDRESSING_MODES[static_cast<size_t>(
                  AddressingModeId::INDEXED_INDIRECT)]},
    Operation{.id = 0x91,
              .instruction = Instructions::STA,
              .cycles = 6,
              .addressingMode = ADDRESSING_MODES[static_cast<size_t>(
                  AddressingModeId::INDIRECT_INDEXED)]},
    Operation{
        .id = 0x86,
        .instruction = Instructions::STX,
        .cycles =
            3,
        .addressingMode =
            ADDRESSING_MODES[static_cast<size_t>(AddressingModeId::ZERO_PAGE)]},
    Operation{.id = 0x96,
              .instruction = Instructions::STX,
              .cycles = 4,
              .addressingMode = ADDRESSING_MODES[static_cast<size_t>(
                  AddressingModeId::INDEXED_ZERO_PAGE_Y)]},
    Operation{
        .id = 0x8e,
        .instruction = Instructions::STX,
        .cycles =
            4,
        .addressingMode =
            ADDRESSING_MODES[static_cast<size_t>(AddressingModeId::ABSOLUTE)]},
    Operation{
        .id = 0x84,
        .instruction = Instructions::STY,
        .cycles =
            3,
        .addressingMode =
            ADDRESSING_MODES[static_cast<size_t>(AddressingModeId::ZERO_PAGE)]},
    Operation{.id = 0x94,
              .instruction = Instructions::STY,
              .cycles = 4,
              .addressingMode = ADDRESSING_MODES[static_cast<size_t>(
                  AddressingModeId::INDEXED_ZERO_PAGE_X)]},
    Operation{
        .id = 0x8c,
        .instruction = Instructions::STY,
        .cycles =
            4,
        .addressingMode =
            ADDRESSING_MODES[static_cast<size_t>(AddressingModeId::ABSOLUTE)]},
    // ----------------- Transfers -----------------
    Operation{
        .id = 0xaa,
        .instruction = Instructions::TAX,
        .cycles =
            2,
        .addressingMode =
            ADDRESSING_MODES[static_cast<size_t>(AddressingModeId::IMPLICIT)]},
    Operation{
        .id = 0xa8,
        .instruction = Instructions::TAY,
        .cycles =
            2,
        .addressingMode =
            ADDRESSING_MODES[static_cast<size_t>(AddressingModeId::IMPLICIT)]},
    Operation{
        .id = 0xba,
        .instruction = Instructions::TSX,
        .cycles =
            2,
        .addressingMode =
            ADDRESSING_MODES[static_cast<size_t>(AddressingModeId::IMPLICIT)]},
    Operation{
        .id = 0x8a,
        .instruction = Instructions::TXA,
        .cycles =
            2,
        .addressingMode =
            ADDRESSING_MODES[static_cast<size_t>(AddressingModeId::IMPLICIT)]},
    Operation{
        .id = 0x9a,
        .instruction = Instructions::TXS,
        .cycles =
            2,
        .addressingMode =
            ADDRESSING_MODES[static_cast<size_t>(AddressingModeId::IMPLICIT)]},
    Operation{
        .id = 0x98,
        .instruction = Instructions::TYA,
        .cycles =
            2,
        .addressingMode =
            ADDRESSING_MODES[static_cast<size_t>(AddressingModeId::IMPLICIT)]},

    // ----------------- illegal opcodes -----------------
    Operation{
        .id = 0x04,
        .instruction = Instructions::NOP,
        .cycles =
            3,
        .addressingMode =
            ADDRESSING_MODES[static_cast<size_t>(AddressingModeId::ZERO_PAGE)]},
    Operation{.id = 0x14,
              .instruction = Instructions::NOP,
              .cycles = 4,
              .addressingMode = ADDRESSING_MODES[static_cast<size_t>(
                  AddressingModeId::INDEXED_ZERO_PAGE_X)]},
    Operation{.id = 0x34,
              .instruction = Instructions::NOP,
              .cycles = 4,
              .addressingMode = ADDRESSING_MODES[static_cast<size_t>(
                  AddressingModeId::INDEXED_ZERO_PAGE_X)]},
    Operation{
        .id = 0x44,
        .instruction = Instructions::NOP,
        .cycles =
            3,
        .addressingMode =
            ADDRESSING_MODES[static_cast<size_t>(AddressingModeId::ZERO_PAGE)]},
    Operation{.id = 0x54,
              .instruction = Instructions::NOP,
              .cycles = 4,
              .addressingMode = ADDRESSING_MODES[static_cast<size_t>(
                  AddressingModeId::INDEXED_ZERO_PAGE_X)]},
    Operation{
        .id = 0x64,
        .instruction = Instructions::NOP,
        .cycles =
            3,
        .addressingMode =
            ADDRESSING_MODES[static_cast<size_t>(AddressingModeId::ZERO_PAGE)]},
    Operation{.id = 0x74,
              .instruction = Instructions::NOP,
              .cycles = 4,
              .addressingMode = ADDRESSING_MODES[static_cast<size_t>(
                  AddressingModeId::INDEXED_ZERO_PAGE_X)]},
    Operation{
        .id = 0x80,
        .instruction = Instructions::NOP,
        .cycles =
            2,
        .addressingMode =
            ADDRESSING_MODES[static_cast<size_t>(AddressingModeId::IMMEDIATE)]},
    Operation{
        .id = 0x82,
        .instruction = Instructions::NOP,
        .cycles =
            2,
        .addressingMode =
            ADDRESSING_MODES[static_cast<size_t>(AddressingModeId::IMMEDIATE)]},
    Operation{
        .id = 0x89,
        .instruction = Instructions::NOP,
        .cycles =
            2,
        .addressingMode =
            ADDRESSING_MODES[static_cast<size_t>(AddressingModeId::IMMEDIATE)]},
    Operation{
        .id = 0xc2,
        .instruction = Instructions::NOP,
        .cycles =
            2,
        .addressingMode =
            ADDRESSING_MODES[static_cast<size_t>(AddressingModeId::IMMEDIATE)]},
    Operation{.id = 0xd4,
              .instruction = Instructions::NOP,
              .cycles = 4,
              .addressingMode = ADDRESSING_MODES[static_cast<size_t>(
                  AddressingModeId::INDEXED_ZERO_PAGE_X)]},
    Operation{
        .id = 0xe2,
        .instruction = Instructions::NOP,
        .cycles =
            2,
        .addressingMode =
            ADDRESSING_MODES[static_cast<size_t>(AddressingModeId::IMMEDIATE)]},
    Operation{.id = 0xf4,
              .instruction = Instructions::NOP,
              .cycles = 4,
              .addressingMode = ADDRESSING_MODES[static_cast<size_t>(
                  AddressingModeId::INDEXED_ZERO_PAGE_X)]},
    Operation{
        .id = 0x0c,
        .instruction = Instructions::NOP,
        .cycles =
            4,
        .addressingMode =
            ADDRESSING_MODES[static_cast<size_t>(AddressingModeId::ABSOLUTE)]},
    Operation{.id = 0x1c,
              .instruction = Instructions::NOP,
              .cycles = 4,
              .addressingMode = ADDRESSING_MODES[static_cast<size_t>(
                  AddressingModeId::INDEXED_ABSOLUTE_X)],
              .hasPageCrossPenalty = true},
    Operation{.id = 0x3c,
              .instruction = Instructions::NOP,
              .cycles = 4,
              .addressingMode = ADDRESSING_MODES[static_cast<size_t>(
                  AddressingModeId::INDEXED_ABSOLUTE_X)],
              .hasPageCrossPenalty = true},
    Operation{.id = 0x5c,
              .instruction = Instructions::NOP,
              .cycles = 4,
              .addressingMode = ADDRESSING_MODES[static_cast<size_t>(
                  AddressingModeId::INDEXED_ABSOLUTE_X)],
              .hasPageCrossPenalty = true},
    Operation{.id = 0x7c,
              .instruction = Instructions::NOP,
              .cycles = 4,
              .addressingMode = ADDRESSING_MODES[static_cast<size_t>(
                  AddressingModeId::INDEXED_ABSOLUTE_X)],
              .hasPageCrossPenalty = true},
    Operation{.id = 0xdc,
              .instruction = Instructions::NOP,
              .cycles = 4,
              .addressingMode = ADDRESSING_MODES[static_cast<size_t>(
                  AddressingModeId::INDEXED_ABSOLUTE_X)],
              .hasPageCrossPenalty = true},
    Operation{.id = 0xfc,
              .instruction = Instructions::NOP,
              .cycles = 4,
              .addressingMode = ADDRESSING_MODES[static_cast<size_t>(
                  AddressingModeId::INDEXED_ABSOLUTE_X)],
              .hasPageCrossPenalty = true},

    // LAX
    Operation{
        .id = 0xa7,
        .instruction = Instructions::LAX,
        .cycles =
            3,
        .addressingMode =
            ADDRESSING_MODES[static_cast<size_t>(AddressingModeId::ZERO_PAGE)]},
    Operation{.id = 0xb7,
              .instruction = Instructions::LAX,
              .cycles = 4,
              .addressingMode = ADDRESSING_MODES[static_cast<size_t>(
                  AddressingModeId::INDEXED_ZERO_PAGE_Y)]},
    Operation{
        .id = 0xaf,
        .instruction = Instructions::LAX,
        .cycles =
            4,
        .addressingMode =
            ADDRESSING_MODES[static_cast<size_t>(AddressingModeId::ABSOLUTE)]},
    Operation{.id = 0xbf,
              .instruction = Instructions::LAX,
              .cycles = 4,
              .addressingMode = ADDRESSING_MODES[static_cast<size_t>(
                  AddressingModeId::INDEXED_ABSOLUTE_Y)],
              .hasPageCrossPenalty = true},
    Operation{.id = 0xa3,
              .instruction = Instructions::LAX,
              .cycles = 6,
              .addressingMode = ADDRESSING_MODES[static_cast<size_t>(
                  AddressingModeId::INDEXED_INDIRECT)]},
    Operation{.id = 0xb3,
              .instruction = Instructions::LAX,
              .cycles = 5,
              .addressingMode = ADDRESSING_MODES[static_cast<size_t>(
                  AddressingModeId::INDIRECT_INDEXED)],
              .hasPageCrossPenalty = true},

    // SAX
    Operation{
        .id = 0x87,
        .instruction = Instructions::SAX,
        .cycles =
            3,
        .addressingMode =
            ADDRESSING_MODES[static_cast<size_t>(AddressingModeId::ZERO_PAGE)]},
    Operation{.id = 0x97,
              .instruction = Instructions::SAX,
              .cycles = 4,
              .addressingMode = ADDRESSING_MODES[static_cast<size_t>(
                  AddressingModeId::INDEXED_ZERO_PAGE_Y)]},
    Operation{
        .id = 0x8f,
        .instruction = Instructions::SAX,
        .cycles =
            4,
        .addressingMode =
            ADDRESSING_MODES[static_cast<size_t>(AddressingModeId::ABSOLUTE)]},
    Operation{.id = 0x83,
              .instruction = Instructions::SAX,
              .cycles = 6,
              .addressingMode = ADDRESSING_MODES[static_cast<size_t>(
                  AddressingModeId::INDEXED_INDIRECT)]},

    // SBC Unofficial Duplicate
    Operation{
        .id = 0xeb,
        .instruction = Instructions::SBC,
        .cycles =
            2,
        .addressingMode =
            ADDRESSING_MODES[static_cast<size_t>(AddressingModeId::IMMEDIATE)]},

    // DCP
    Operation{
        .id = 0xc7,
        .instruction = Instructions::DCP,
        .cycles =
            5,
        .addressingMode =
            ADDRESSING_MODES[static_cast<size_t>(AddressingModeId::ZERO_PAGE)]},
    Operation{.id = 0xd7,
              .instruction = Instructions::DCP,
              .cycles = 6,
              .addressingMode = ADDRESSING_MODES[static_cast<size_t>(
                  AddressingModeId::INDEXED_ZERO_PAGE_X)]},
    Operation{
        .id = 0xcf,
        .instruction = Instructions::DCP,
        .cycles =
            6,
        .addressingMode =
            ADDRESSING_MODES[static_cast<size_t>(AddressingModeId::ABSOLUTE)]},
    Operation{.id = 0xdf,
              .instruction = Instructions::DCP,
              .cycles = 7,
              .addressingMode = ADDRESSING_MODES[static_cast<size_t>(
                  AddressingModeId::INDEXED_ABSOLUTE_X)],
              .hasPageCrossPenalty = true},
    Operation{.id = 0xdb,
              .instruction = Instructions::DCP,
              .cycles = 7,
              .addressingMode = ADDRESSING_MODES[static_cast<size_t>(
                  AddressingModeId::INDEXED_ABSOLUTE_Y)],
              .hasPageCrossPenalty = true},
    Operation{.id = 0xc3,
              .instruction = Instructions::DCP,
              .cycles = 8,
              .addressingMode = ADDRESSING_MODES[static_cast<size_t>(
                  AddressingModeId::INDEXED_INDIRECT)]},
    Operation{.id = 0xd3,
              .instruction = Instructions::DCP,
              .cycles = 8,
              .addressingMode = ADDRESSING_MODES[static_cast<size_t>(
                  AddressingModeId::INDIRECT_INDEXED)],
              .hasPageCrossPenalty = true},

    // ISB
    Operation{
        .id = 0xe7,
        .instruction = Instructions::ISB,
        .cycles =
            5,
        .addressingMode =
            ADDRESSING_MODES[static_cast<size_t>(AddressingModeId::ZERO_PAGE)]},
    Operation{.id = 0xf7,
              .instruction = Instructions::ISB,
              .cycles = 6,
              .addressingMode = ADDRESSING_MODES[static_cast<size_t>(
                  AddressingModeId::INDEXED_ZERO_PAGE_X)]},
    Operation{
        .id = 0xef,
        .instruction = Instructions::ISB,
        .cycles =
            6,
        .addressingMode =
            ADDRESSING_MODES[static_cast<size_t>(AddressingModeId::ABSOLUTE)]},
    Operation{.id = 0xff,
              .instruction = Instructions::ISB,
              .cycles = 7,
              .addressingMode = ADDRESSING_MODES[static_cast<size_t>(
                  AddressingModeId::INDEXED_ABSOLUTE_X)],
              .hasPageCrossPenalty = true},
    Operation{.id = 0xfb,
              .instruction = Instructions::ISB,
              .cycles = 7,
              .addressingMode = ADDRESSING_MODES[static_cast<size_t>(
                  AddressingModeId::INDEXED_ABSOLUTE_Y)],
              .hasPageCrossPenalty = true},
    Operation{.id = 0xe3,
              .instruction = Instructions::ISB,
              .cycles = 8,
              .addressingMode = ADDRESSING_MODES[static_cast<size_t>(
                  AddressingModeId::INDEXED_INDIRECT)]},
    Operation{.id = 0xf3,
              .instruction = Instructions::ISB,
              .cycles = 8,
              .addressingMode = ADDRESSING_MODES[static_cast<size_t>(
                  AddressingModeId::INDIRECT_INDEXED)],
              .hasPageCrossPenalty = true},

    // SLO
    Operation{
        .id = 0x07,
        .instruction = Instructions::SLO,
        .cycles =
            5,
        .addressingMode =
            ADDRESSING_MODES[static_cast<size_t>(AddressingModeId::ZERO_PAGE)]},
    Operation{.id = 0x17,
              .instruction = Instructions::SLO,
              .cycles = 6,
              .addressingMode = ADDRESSING_MODES[static_cast<size_t>(
                  AddressingModeId::INDEXED_ZERO_PAGE_X)]},
    Operation{
        .id = 0x0f,
        .instruction = Instructions::SLO,
        .cycles =
            6,
        .addressingMode =
            ADDRESSING_MODES[static_cast<size_t>(AddressingModeId::ABSOLUTE)]},
    Operation{.id = 0x1f,
              .instruction = Instructions::SLO,
              .cycles = 7,
              .addressingMode = ADDRESSING_MODES[static_cast<size_t>(
                  AddressingModeId::INDEXED_ABSOLUTE_X)],
              .hasPageCrossPenalty = true},
    Operation{.id = 0x1b,
              .instruction = Instructions::SLO,
              .cycles = 7,
              .addressingMode = ADDRESSING_MODES[static_cast<size_t>(
                  AddressingModeId::INDEXED_ABSOLUTE_Y)],
              .hasPageCrossPenalty = true},
    Operation{.id = 0x03,
              .instruction = Instructions::SLO,
              .cycles = 8,
              .addressingMode = ADDRESSING_MODES[static_cast<size_t>(
                  AddressingModeId::INDEXED_INDIRECT)]},
    Operation{.id = 0x13,
              .instruction = Instructions::SLO,
              .cycles = 8,
              .addressingMode = ADDRESSING_MODES[static_cast<size_t>(
                  AddressingModeId::INDIRECT_INDEXED)],
              .hasPageCrossPenalty = true},

    // RLA
    Operation{
        .id = 0x27,
        .instruction = Instructions::RLA,
        .cycles =
            5,
        .addressingMode =
            ADDRESSING_MODES[static_cast<size_t>(AddressingModeId::ZERO_PAGE)]},
    Operation{.id = 0x37,
              .instruction = Instructions::RLA,
              .cycles = 6,
              .addressingMode = ADDRESSING_MODES[static_cast<size_t>(
                  AddressingModeId::INDEXED_ZERO_PAGE_X)]},
    Operation{
        .id = 0x2f,
        .instruction = Instructions::RLA,
        .cycles =
            6,
        .addressingMode =
            ADDRESSING_MODES[static_cast<size_t>(AddressingModeId::ABSOLUTE)]},
    Operation{.id = 0x3f,
              .instruction = Instructions::RLA,
              .cycles = 7,
              .addressingMode = ADDRESSING_MODES[static_cast<size_t>(
                  AddressingModeId::INDEXED_ABSOLUTE_X)],
              .hasPageCrossPenalty = true},
    Operation{.id = 0x3b,
              .instruction = Instructions::RLA,
              .cycles = 7,
              .addressingMode = ADDRESSING_MODES[static_cast<size_t>(
                  AddressingModeId::INDEXED_ABSOLUTE_Y)],
              .hasPageCrossPenalty = true},
    Operation{.id = 0x23,
              .instruction = Instructions::RLA,
              .cycles = 8,
              .addressingMode = ADDRESSING_MODES[static_cast<size_t>(
                  AddressingModeId::INDEXED_INDIRECT)]},
    Operation{.id = 0x33,
              .instruction = Instructions::RLA,
              .cycles = 8,
              .addressingMode = ADDRESSING_MODES[static_cast<size_t>(
                  AddressingModeId::INDIRECT_INDEXED)],
              .hasPageCrossPenalty = true},
    Operation{.id = 0x57,
              .instruction = Instructions::SRE,
              .cycles = 6,
              .addressingMode = ADDRESSING_MODES[static_cast<size_t>(
                  AddressingModeId::INDEXED_ZERO_PAGE_X)]},
    Operation{
        .id = 0x4f,
        .instruction = Instructions::SRE,
        .cycles =
            6,
        .addressingMode =
            ADDRESSING_MODES[static_cast<size_t>(AddressingModeId::ABSOLUTE)]},
    Operation{.id = 0x5f,
              .instruction = Instructions::SRE,
              .cycles = 7,
              .addressingMode = ADDRESSING_MODES[static_cast<size_t>(
                  AddressingModeId::INDEXED_ABSOLUTE_X)],
              .hasPageCrossPenalty = true},
    Operation{.id = 0x5b,
              .instruction = Instructions::SRE,
              .cycles = 7,
              .addressingMode = ADDRESSING_MODES[static_cast<size_t>(
                  AddressingModeId::INDEXED_ABSOLUTE_Y)],
              .hasPageCrossPenalty = true},
    Operation{.id = 0x43,
              .instruction = Instructions::SRE,
              .cycles = 8,
              .addressingMode = ADDRESSING_MODES[static_cast<size_t>(
                  AddressingModeId::INDEXED_INDIRECT)]},
    Operation{.id = 0x53,
              .instruction = Instructions::SRE,
              .cycles = 8,
              .addressingMode = ADDRESSING_MODES[static_cast<size_t>(
                  AddressingModeId::INDIRECT_INDEXED)],
              .hasPageCrossPenalty = true},
    // RRA
    Operation{
        .id = 0x67,
        .instruction = Instructions::RRA,
        .cycles =
            5,
        .addressingMode =
            ADDRESSING_MODES[static_cast<size_t>(AddressingModeId::ZERO_PAGE)]},
    Operation{.id = 0x77,
              .instruction = Instructions::RRA,
              .cycles = 6,
              .addressingMode = ADDRESSING_MODES[static_cast<size_t>(
                  AddressingModeId::INDEXED_ZERO_PAGE_X)]},
    Operation{
        .id = 0x6f,
        .instruction = Instructions::RRA,
        .cycles =
            6,
        .addressingMode =
            ADDRESSING_MODES[static_cast<size_t>(AddressingModeId::ABSOLUTE)]},
    Operation{.id = 0x7f,
              .instruction = Instructions::RRA,
              .cycles = 7,
              .addressingMode = ADDRESSING_MODES[static_cast<size_t>(
                  AddressingModeId::INDEXED_ABSOLUTE_X)],
              .hasPageCrossPenalty = true},
    Operation{.id = 0x7b,
              .instruction = Instructions::RRA,
              .cycles = 7,
              .addressingMode = ADDRESSING_MODES[static_cast<size_t>(
                  AddressingModeId::INDEXED_ABSOLUTE_Y)],
              .hasPageCrossPenalty = true},
    // SRE
    Operation{
        .id = 0x47,
        .instruction = Instructions::SRE,
        .cycles =
            5,
        .addressingMode =
            ADDRESSING_MODES[static_cast<size_t>(AddressingModeId::ZERO_PAGE)]},
    Operation{.id = 0x63,
              .instruction = Instructions::RRA,
              .cycles = 8,
              .addressingMode = ADDRESSING_MODES[static_cast<size_t>(
                  AddressingModeId::INDEXED_INDIRECT)]},
    Operation{.id = 0x73,
              .instruction = Instructions::RRA,
              .cycles = 8,
              .addressingMode = ADDRESSING_MODES[static_cast<size_t>(
                  AddressingModeId::INDIRECT_INDEXED)],
              .hasPageCrossPenalty = true},
    // Immediate Opcodes
    Operation{
        .id = 0x0b,
        .instruction = Instructions::AAC,
        .cycles =
            2,
        .addressingMode =
            ADDRESSING_MODES[static_cast<size_t>(AddressingModeId::IMMEDIATE)]},
    Operation{
        .id = 0x2b,
        .instruction = Instructions::AAC,
        .cycles =
            2,
        .addressingMode =
            ADDRESSING_MODES[static_cast<size_t>(AddressingModeId::IMMEDIATE)]},
    Operation{
        .id = 0x4b,
        .instruction = Instructions::ASR,
        .cycles =
            2,
        .addressingMode =
            ADDRESSING_MODES[static_cast<size_t>(AddressingModeId::IMMEDIATE)]},
    Operation{
        .id = 0x6b,
        .instruction = Instructions::ARR,
        .cycles =
            2,
        .addressingMode =
            ADDRESSING_MODES[static_cast<size_t>(AddressingModeId::IMMEDIATE)]},
    Operation{
        .id = 0xab,
        .instruction = Instructions::ATX,
        .cycles =
            2,
        .addressingMode =
            ADDRESSING_MODES[static_cast<size_t>(AddressingModeId::IMMEDIATE)]},
    Operation{
        .id = 0xcb,
        .instruction = Instructions::AXS,
        .cycles =
            2,
        .addressingMode =
            ADDRESSING_MODES[static_cast<size_t>(AddressingModeId::IMMEDIATE)]},

    Operation{.id = 0x9c,
              .instruction = Instructions::SYA,
              .cycles = 5,
              .addressingMode = ADDRESSING_MODES[static_cast<size_t>(
                  AddressingModeId::INDEXED_ABSOLUTE_X)],
              .hasPageCrossPenalty = false},
    Operation{.id = 0x9e,
              .instruction = Instructions::SXA,
              .cycles = 5,
              .addressingMode = ADDRESSING_MODES[static_cast<size_t>(
                  AddressingModeId::INDEXED_ABSOLUTE_Y)],
              .hasPageCrossPenalty = false},
};

constexpr std::array<Operation, 256>
generateLookupMatrix(const std::array<Operation, 256> &source) {
  std::array<Operation, 256> matrix{};
  for (size_t i = 0; i < 256; ++i) {
    matrix[i] = Operation{
        .id = static_cast<u8>(i),
        .instruction = Instructions::NOP,
        .cycles = 2,
        .addressingMode =
            ADDRESSING_MODES[static_cast<size_t>(AddressingModeId::IMPLICIT)],
        .hasPageCrossPenalty = false};
  }
  for (size_t i = 0; i < source.size(); ++i) {
    const auto &op = source[i];
    if (i == 0 || op.addressingMode != nullptr) {
      matrix[op.id] = op;
    }
  }
  return matrix;
}

const std::array<Operation, 256> OPERATIONS = generateLookupMatrix(RAW_LIST);
