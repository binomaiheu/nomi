cmake_minimum_required(VERSION 3.21...3.31)

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

option(NOMI_BUILD_TESTS "Build the unit tests" ON)

# dependencies are declared in vcpkg.json
find_package(SFML 3 COMPONENTS Graphics Audio REQUIRED)

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

# everything except main() lives in a library so the tests can link it
add_library(nomi_core STATIC
  src/animation.cpp
  src/audio.cpp
  src/autotest.cpp
  src/collisionmap.cpp
  src/enemy.cpp
  src/entity.cpp
  src/game.cpp
  src/hud.cpp
  src/levelmap.cpp
  src/pickup.cpp
  src/player.cpp
  src/scenenode.cpp
  src/session.cpp
  src/state.cpp
  src/states.cpp
  src/tilemaplayer.cpp
  src/utils.cpp
  src/world.cpp
)

target_include_directories(nomi_core PUBLIC src)

target_compile_features(nomi_core PUBLIC cxx_std_17)

target_compile_options(nomi_core
  PUBLIC
  $<$<CXX_COMPILER_ID:GNU,Clang>:-Wall -Wextra -Wno-switch -Wno-unused>
  $<$<CXX_COMPILER_ID:MSVC>:/W3 /utf-8 /wd4244 /wd4267>
)

target_compile_definitions(nomi_core
  PUBLIC
  $<$<CONFIG:Debug>:DEBUG>
)

target_link_libraries(nomi_core
  PUBLIC
  SFML::Graphics SFML::Audio
  tmxlite
)

add_executable(nomi src/main.cpp)
target_link_libraries(nomi PRIVATE nomi_core)

# copy the assets next to the executable so it also runs outside the source tree
add_custom_command(TARGET nomi POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different
          "${CMAKE_CURRENT_SOURCE_DIR}/share" "$<TARGET_FILE_DIR:nomi>/share"
)

set_target_properties(nomi PROPERTIES
  VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
)

