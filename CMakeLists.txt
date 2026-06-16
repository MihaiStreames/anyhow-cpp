cmake_minimum_required(VERSION 3.22)
project(
  anyhow-cpp
  VERSION 0.1.0
  DESCRIPTION "Rust's anyhow, ported to C++20."
  LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Library target

add_library(anyhow INTERFACE)
add_library(anyhow::anyhow ALIAS anyhow)

target_include_directories(
  anyhow INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
                   $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)

target_compile_features(anyhow INTERFACE cxx_std_20)

# Install rules

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

install(DIRECTORY src/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

install(TARGETS anyhow EXPORT anyhow-targets)

install(
  EXPORT anyhow-targets
  FILE anyhow-targets.cmake
  NAMESPACE anyhow::
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/anyhow)

configure_package_config_file(
  cmake/anyhow-config.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/anyhow-config.cmake
  INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/anyhow)

write_basic_package_version_file(
  ${CMAKE_CURRENT_BINARY_DIR}/anyhow-config-version.cmake
  VERSION ${PROJECT_VERSION}
  COMPATIBILITY SameMajorVersion)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/anyhow-config.cmake
              ${CMAKE_CURRENT_BINARY_DIR}/anyhow-config-version.cmake
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/anyhow)

# Tests (opt-in)

option(ANYHOW_BUILD_TESTS "Build anyhow-cpp tests" OFF)

if(ANYHOW_BUILD_TESTS)
  enable_testing()
  add_subdirectory(tests)
endif()
