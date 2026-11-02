add_library(nes_component_config INTERFACE)

target_link_libraries(nes_component_config INTERFACE
  SDL2::SDL2
  fmt::fmt
  imgui
)

target_include_directories(nes_component_config INTERFACE
  ${PROJECT_SOURCE_DIR}/include
  ${PROJECT_SOURCE_DIR}/src
)

add_subdirectory(apu)
add_subdirectory(cpu)
add_subdirectory(mappers)
add_subdirectory(platform)
add_subdirectory(ppu)

add_library(nes_core STATIC)

target_sources(nes_core PRIVATE
  cartridge.cpp
  rom_loader.cpp
  $<TARGET_OBJECTS:nes_apu>
  $<TARGET_OBJECTS:nes_cpu>
  $<TARGET_OBJECTS:nes_mappers>
  $<TARGET_OBJECTS:nes_ppu>
  $<TARGET_OBJECTS:nes_platform>
)

target_link_libraries(nes_core PUBLIC nes_component_config)

target_compile_definitions(nes_component_config INTERFACE
  $<$<CONFIG:Debug>:DEFAULT_SDL_LOG_LEVEL=2>
  $<$<CONFIG:Info>:DEFAULT_SDL_LOG_LEVEL=3>
  $<$<CONFIG:Release>:DEFAULT_SDL_LOG_LEVEL=4>
  $<$<CONFIG:MinSizeRel>:DEFAULT_SDL_LOG_LEVEL=5>
)
