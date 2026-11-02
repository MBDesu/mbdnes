
add_library(nes_platform OBJECT)
target_sources(nes_platform PRIVATE
  controller.cpp
  platform.cpp
)
target_link_libraries(nes_platform PRIVATE nes_component_config)
