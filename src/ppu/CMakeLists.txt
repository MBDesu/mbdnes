add_library(nes_ppu OBJECT)
target_sources(nes_ppu PRIVATE
  ppu_memory.cpp
  ppu.cpp
  video_registers.cpp
)
target_link_libraries(nes_ppu PRIVATE nes_component_config)
