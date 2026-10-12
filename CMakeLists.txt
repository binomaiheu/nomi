cmake_minimum_required(VERSION 3.16...3.31)

# locate the vcpkg toolchain : -DCMAKE_TOOLCHAIN_FILE=... takes precedence, then
# the VCPKG_ROOT environment variable, then a vcpkg checkout next to this project
if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
  if(DEFINED ENV{VCPKG_ROOT})
    set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")
  elseif(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/../vcpkg/scripts/buildsystems/vcpkg.cmake")
    set(CMAKE_TOOLCHAIN_FILE "${CMAKE_CURRENT_SOURCE_DIR}/../vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")
  endif()
endif()

project(nomi CXX)

# dependencies are declared in vcpkg.json
find_package(SFML 2.5 COMPONENTS system window graphics audio network REQUIRED)
find_package(Boost REQUIRED COMPONENTS program_options)
# the vcpkg tmxlite port ships no cmake config, locate it by hand
find_path(TMXLITE_INCLUDE_DIR tmxlite/Map.hpp REQUIRED)
find_library(TMXLITE_LIBRARY_RELEASE NAMES tmxlite REQUIRED)
find_library(TMXLITE_LIBRARY_DEBUG NAMES tmxlite-d tmxlite)
add_library(tmxlite UNKNOWN IMPORTED)
set_target_properties(tmxlite PROPERTIES
  INTERFACE_INCLUDE_DIRECTORIES "${TMXLITE_INCLUDE_DIR}"
  IMPORTED_LOCATION "${TMXLITE_LIBRARY_RELEASE}"
  IMPORTED_LOCATION_DEBUG "${TMXLITE_LIBRARY_DEBUG}"
)

add_executable(nomi
  src/main.cpp
  src/utils.cpp
  src/game.cpp
  src/scenenode.cpp
  src/entity.cpp
  src/player.cpp
  src/animation.cpp
  src/levelmap.cpp
  src/tilemaplayer.cpp
  src/world.cpp
  src/solid.cpp
)

target_compile_features(nomi
  PRIVATE
  cxx_std_17
)

target_compile_options(nomi
  PRIVATE
  $<$<CXX_COMPILER_ID:GNU>:-Wall -Wextra -Wno-switch -Wno-unused>
  $<$<CXX_COMPILER_ID:MSVC>:/W3 /utf-8>
)

target_compile_definitions(nomi
  PRIVATE
  $<$<CONFIG:Debug>:DEBUG>
)

target_link_libraries(nomi
  PRIVATE
  sfml-graphics sfml-window sfml-system sfml-audio sfml-network
  Boost::program_options
  tmxlite
)

# the game loads its assets from share/ relative to the working directory
set_target_properties(nomi PROPERTIES
  VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
)
