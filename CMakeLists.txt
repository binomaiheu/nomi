cmake_minimum_required(VERSION 3.11)

set(CMAKE_TOOLCHAIN_FILE "/home/bino/Development/vcpkg/scripts/buildsystems/vcpkg.cmake")

if(DEFINED ENV{VCPKG_ROOT} AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
  set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake"
      CACHE STRING "")
endif()

project(nomi)

set(CMAKE_MODULE_PATH
    "${CMAKE_SOURCE_DIR}/cmake_modules"
     ${CMAKE_MODULE_PATH})

# for static libs uncomment here, but then ze need static smfl with all deps...
#set(SFML_STATIC_LIBRARIES TRUE)


# I have a somewhat older graphics card, so need to use the legacy interface
set(OpenGL_GL_PREFERENCE LEGACY)
cmake_policy(SET CMP0072 OLD)

find_package(SFML 2.5 COMPONENTS system window graphics audio network REQUIRED)
find_package(OpenGL)
find_package(Boost REQUIRED COMPONENTS date_time system filesystem program_options)
find_package(Eigen3 REQUIRED)


add_executable(nomi
  src/main.cpp
  src/game.cpp
  src/player.cpp
)

target_compile_features(nomi
  PRIVATE
  cxx_std_17
)

target_compile_options(nomi
  PRIVATE
  $<$<CXX_COMPILER_ID:GNU>:-Wall -Wextra -Wno-switch>
)

target_compile_definitions(nomi
  PRIVATE
  $<$<CONFIG:Debug>:DEBUG>
)

target_link_libraries(nomi
  PRIVATE
  sfml-graphics sfml-window sfml-system sfml-audio sfml-network
  OpenGL::GL
  Boost::date_time
  Boost::program_options
  Boost::system
  Eigen3::Eigen
)
