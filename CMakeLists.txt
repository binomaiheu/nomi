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

project(nomi
  VERSION 1.0.2
  DESCRIPTION "A little platformer, levels made with Tiled"
  HOMEPAGE_URL "https://github.com/binomaiheu/nomi"
  LANGUAGES CXX
)

option(NOMI_BUILD_TESTS "Build the unit tests" ON)
option(NOMI_WINDOWED "Build nomi.exe as a windowed application without a console (for releases)" OFF)

# a static vcpkg triplet (x64-windows-static) gives a single nomi.exe without DLLs
if(VCPKG_TARGET_TRIPLET MATCHES "static")
  set(NOMI_STATIC ON)
  set(SFML_STATIC_LIBRARIES ON)
endif()

# dependencies are declared in vcpkg.json
find_package(SFML 3 COMPONENTS Graphics Audio Main REQUIRED)

# the vcpkg tmxlite port ships no cmake config, locate it and its dependencies by hand
find_package(pugixml CONFIG REQUIRED)
find_package(ZLIB REQUIRED)
find_package(zstd CONFIG REQUIRED)
find_path(TMXLITE_INCLUDE_DIR tmxlite/Map.hpp REQUIRED)
find_library(TMXLITE_LIBRARY_RELEASE NAMES tmxlite tmxlite-s REQUIRED)
find_library(TMXLITE_LIBRARY_DEBUG NAMES tmxlite-d tmxlite-s-d tmxlite tmxlite-s)
add_library(tmxlite UNKNOWN IMPORTED)
set_target_properties(tmxlite PROPERTIES
  INTERFACE_INCLUDE_DIRECTORIES "${TMXLITE_INCLUDE_DIR}"
  IMPORTED_LOCATION "${TMXLITE_LIBRARY_RELEASE}"
  IMPORTED_LOCATION_DEBUG "${TMXLITE_LIBRARY_DEBUG}"
  INTERFACE_LINK_LIBRARIES "pugixml::pugixml;ZLIB::ZLIB;$<IF:$<TARGET_EXISTS:zstd::libzstd_shared>,zstd::libzstd_shared,zstd::libzstd_static>"
)
if(NOMI_STATIC)
  set_property(TARGET tmxlite APPEND PROPERTY INTERFACE_COMPILE_DEFINITIONS TMXLITE_STATIC)
endif()

# version number for the code
configure_file(resources/version.hpp.in "${CMAKE_CURRENT_BINARY_DIR}/generated/version.hpp" @ONLY)

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

target_include_directories(nomi_core PUBLIC src "${CMAKE_CURRENT_BINARY_DIR}/generated")

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

if(WIN32)
  # icon and version info of nomi.exe
  enable_language(RC)
  set(NOMI_ICON "${CMAKE_CURRENT_SOURCE_DIR}/resources/nomi.ico")
  configure_file(resources/nomi.rc.in "${CMAKE_CURRENT_BINARY_DIR}/generated/nomi.rc" @ONLY)
  target_sources(nomi PRIVATE "${CMAKE_CURRENT_BINARY_DIR}/generated/nomi.rc")
endif()

if(NOMI_WINDOWED)
  # no console window; SFML::Main provides the WinMain entry point that calls main()
  set_target_properties(nomi PROPERTIES WIN32_EXECUTABLE ON)
  target_link_libraries(nomi PRIVATE SFML::Main)
endif()

# copy the assets next to the executable so it also runs outside the source tree
add_custom_command(TARGET nomi POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different
          "${CMAKE_CURRENT_SOURCE_DIR}/share" "$<TARGET_FILE_DIR:nomi>/share"
)

set_target_properties(nomi PROPERTIES
  VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
)

if(NOMI_BUILD_TESTS)
  enable_testing()
  find_package(doctest CONFIG REQUIRED)
  add_executable(nomi_tests
    tests/main.cpp
    tests/test_collision.cpp
    tests/test_rules.cpp
    tests/test_levels.cpp
  )
  target_link_libraries(nomi_tests PRIVATE nomi_core doctest::doctest)
  target_compile_definitions(nomi_tests PRIVATE NOMI_SOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}")
  add_test(NAME unit_tests COMMAND nomi_tests)
  # headless playthroughs : the bot has to reach the flag in every level
  add_test(NAME autotest_level1 COMMAND nomi --autotest --level "${CMAKE_CURRENT_SOURCE_DIR}/share/levels/level1.tmx")
  add_test(NAME autotest_level2 COMMAND nomi --autotest --level "${CMAKE_CURRENT_SOURCE_DIR}/share/levels/level2.tmx")
endif()

# ---------------------------------------------------------------- install & package
#
#   cmake --preset windows-static
#   cmake --build --preset package
#   cpack --preset package          -> build-static/nomi-<version>-win64.zip and .exe installer
#
# The installed layout is nomi.exe with share/ next to it.

install(TARGETS nomi RUNTIME DESTINATION .)
install(DIRECTORY share/ DESTINATION share)
install(FILES README.md DESTINATION .)
if(NOT NOMI_STATIC)
  # a DLL build : ship the DLLs vcpkg copied next to the executable
  install(DIRECTORY "$<TARGET_FILE_DIR:nomi>/" DESTINATION .
          FILES_MATCHING PATTERN "*.dll" PATTERN "share" EXCLUDE)
endif()

set(CPACK_PACKAGE_NAME "nomi")
set(CPACK_PACKAGE_VENDOR "Bino Maiheu")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "${PROJECT_DESCRIPTION}")
set(CPACK_PACKAGE_INSTALL_DIRECTORY "nomi")
set(CPACK_PACKAGE_EXECUTABLES "nomi" "nomi")                 # start menu shortcut
set(CPACK_SYSTEM_NAME "win64")
set(CPACK_PACKAGE_CHECKSUM SHA256)
set(CPACK_STRIP_FILES ON)

# NSIS installer : setup.exe with shortcuts and an uninstaller
set(CPACK_NSIS_DISPLAY_NAME "nomi ${PROJECT_VERSION}")
set(CPACK_NSIS_PACKAGE_NAME "nomi")
set(CPACK_NSIS_MUI_ICON "${CMAKE_CURRENT_SOURCE_DIR}/resources/nomi.ico")
set(CPACK_NSIS_MUI_UNIICON "${CMAKE_CURRENT_SOURCE_DIR}/resources/nomi.ico")
set(CPACK_NSIS_INSTALLED_ICON_NAME "nomi.exe")
set(CPACK_NSIS_URL_INFO_ABOUT "${PROJECT_HOMEPAGE_URL}")
set(CPACK_NSIS_ENABLE_UNINSTALL_BEFORE_INSTALL ON)
set(CPACK_NSIS_MUI_FINISHPAGE_RUN "nomi.exe")
set(CPACK_CREATE_DESKTOP_LINKS "nomi")
set(CPACK_NSIS_MANIFEST_DPI_AWARE ON)

set(CPACK_GENERATOR "ZIP;NSIS")
include(CPack)
