add_library(nes_mappers OBJECT)
target_sources(nes_mappers PRIVATE
  0_NROM.cpp
  1_MMC1.cpp
  2_UxROM.cpp
  3_CNROM.cpp
  4_MMC3.cpp
  mapper.cpp
)
target_link_libraries(nes_mappers PRIVATE nes_component_config)
