# set(CMAKE_CXX_COMPILER /opt/homebrew/bin/g++-16)
cmake_minimum_required(VERSION 3.24)
project(mbdnes VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(SDL2 REQUIRED CONFIG)
find_package(fmt REQUIRED)

include(FetchContent)
set(IMGUI_VERSION "1.91.5")
set(imgui_BUILD_SUBDIRECTORY OFF)

FetchContent_Declare(
  imgui
  URL "https://github.com/ocornut/imgui/archive/refs/tags/v${IMGUI_VERSION}-docking.tar.gz"
  EXCLUDE_FROM_ALL
)
FetchContent_MakeAvailable(imgui)

FetchContent_Declare(
  Catch2
  GIT_REPOSITORY "https://github.com/catchorg/Catch2.git"
  GIT_TAG v3.16.1
)
FetchContent_MakeAvailable(Catch2)

add_library(imgui STATIC)
target_sources(imgui PRIVATE
  "${imgui_SOURCE_DIR}/imgui.cpp"
  "${imgui_SOURCE_DIR}/imgui_demo.cpp"
  "${imgui_SOURCE_DIR}/imgui_draw.cpp"
  "${imgui_SOURCE_DIR}/imgui_tables.cpp"
  "${imgui_SOURCE_DIR}/imgui_widgets.cpp"
  "${imgui_SOURCE_DIR}/backends/imgui_impl_sdl2.cpp"
  "${imgui_SOURCE_DIR}/backends/imgui_impl_sdlrenderer2.cpp"
)
target_include_directories(imgui PUBLIC "${imgui_SOURCE_DIR}" "${imgui_SOURCE_DIR}/backends")
target_link_libraries(imgui PUBLIC SDL2::SDL2)

add_subdirectory(src)
add_executable(mbdnes main.cpp)
target_link_libraries(mbdnes PRIVATE nes_core)
target_include_directories(mbdnes PUBLIC "${CMAKE_SOURCE_DIR}/include")

enable_testing()
find_package(Catch2 3 REQUIRED)
add_subdirectory(tests)

if(APPLE)
  add_custom_command(TARGET mbdnes POST_BUILD
    COMMAND install_name_tool -change
    "@rpath/SDL2.framework/Versions/A/SDL2"
    "/opt/homebrew/lib/libSDL2-2.0.dylib"
    "$<TARGET_FILE:mbdnes>"
    COMMENT "Fixing hardcoded SDL2 framework path for dyld..."
  )

  if(TARGET unit_tests)
    add_custom_command(TARGET mbdnes POST_BUILD
      COMMAND install_name_tool -change
      "@rpath/SDL2.framework/Versions/A/SDL2"
      "/opt/homebrew/lib/libSDL2-2.0.dylib"
      "$<TARGET_FILE:unit_tests>"
      COMMENT "Fixing hardcoded SDL2 framework path for unit tests..."
    )
  endif()
endif()

target_include_directories(nes_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../include)
target_link_libraries(nes_core PUBLIC SDL2::SDL2 fmt::fmt imgui)

if(APPLE)
  set_target_properties(mbdnes PROPERTIES
    BUILD_WITH_INSTALL_RPATH TRUE
    INSTALL_RPATH "/opt/homebrew/lib"
  )
  
  if(TARGET unit_tests)
    set_target_properties(unit_tests PROPERTIES
      BUILD_WITH_INSTALL_RPATH TRUE
      INSTALL_RPATH "/opt/homebrew/lib"
    )
  endif()
endif()
