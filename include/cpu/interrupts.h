#pragma once
#ifndef MODULE_CPU_INTERRUPTS_H
#define MODULE_CPU_INTERRUPTS_H

#include "globals.h"

#include <array>
#include <string_view>

enum Interrupts { NMI, RESET, IRQ, BRK, NUM_INTERRUPTS };

struct Interrupt {
  std::string_view id;
  u16 vector;
};

constexpr std::array<Interrupt, static_cast<size_t>(Interrupts::NUM_INTERRUPTS)>
    INTERRUPTS = {
        {{"NMI", 0xfffa}, {"RESET", 0xfffc}, {"IRQ", 0xfffe}, {"BRK", 0xfffe}}};

#endif // MODULE_CPU_INTERRUPTS_H
