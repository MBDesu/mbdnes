add_library(nes_cpu OBJECT)
target_sources(nes_cpu PRIVATE
  addressing_modes.cpp
  cpu_memory.cpp
  cpu.cpp
  disassembler.cpp
  instructions.cpp
  operations.cpp
)
target_link_libraries(nes_cpu PRIVATE nes_component_config)
