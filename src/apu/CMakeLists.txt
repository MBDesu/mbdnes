add_library(nes_apu OBJECT)
target_sources(nes_apu PRIVATE
  apu.cpp
  audio_registers.cpp
  dmc_channel.cpp
  dpcm.cpp
  frame_sequencer.cpp
  frequency_sweep.cpp
  length_counter.cpp
  noise_channel.cpp
  pulse_channel.cpp
  triangle_channel.cpp
  volume_envelope.cpp
)
target_link_libraries(nes_apu PRIVATE nes_component_config)
