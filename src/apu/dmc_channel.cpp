#include "apu/dmc_channel.h"
#include "apu/apu.h"
#include "apu/dpcm.h"

DMCChannel::DMCChannel(APU *apu, CPU *cpu) : m_apu(apu), m_cpu(cpu) {
  m_dpcm = new DPCM(this);
}

DMCChannel::~DMCChannel() { delete m_dpcm; }

DMCRegisters &DMCChannel::getRegisters() const {
  return m_apu->m_registers.m_dmc;
}

void DMCChannel::step() { m_dpcm->update(); }

u32 DMCChannel::getRemainingBytes() const { return m_dpcm->remainingBytes(); }

void DMCChannel::startDPCM() { m_dpcm->start(); }

void DMCChannel::stopDPCM() { m_dpcm->stop(); }
