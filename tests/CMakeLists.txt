
add_executable(unit_tests)

target_sources(unit_tests PRIVATE
  cpu_test.cpp
  ppu_test.cpp
)

target_link_libraries(unit_tests PRIVATE
  nes_core
  Catch2::Catch2WithMain
)

include(Catch)
catch_discover_tests(unit_tests DISCOVERY_MODE PRE_TEST)
