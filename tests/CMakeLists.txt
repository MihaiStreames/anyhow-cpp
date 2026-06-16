include(FetchContent)

FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG v1.15.2)

set(gtest_force_shared_crt
    ON
    CACHE BOOL "" FORCE)
set(BUILD_GMOCK
    OFF
    CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# Unit tests ###

set(ANYHOW_TEST_SOURCES test_expected.cpp test_context.cpp test_fmt.cpp
                        test_scope_guard.cpp test_macros.cpp test_failure.cpp)

add_executable(anyhow-tests ${ANYHOW_TEST_SOURCES})

target_link_libraries(anyhow-tests PRIVATE anyhow::anyhow GTest::gtest_main)
target_compile_features(anyhow-tests PRIVATE cxx_std_20)
target_compile_definitions(anyhow-tests PRIVATE ANYHOW_SHORT_MACROS)

# __VA_OPT__ is C++20 but MSVC requires an explicit conformance flag to enable
# it
if(MSVC)
  target_compile_options(anyhow-tests PRIVATE /Zc:preprocessor)
endif()
target_include_directories(anyhow-tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

include(GoogleTest)
gtest_discover_tests(anyhow-tests)

# Fuzz target (clang libFuzzer only) ###

option(ANYHOW_BUILD_FUZZ
       "Build libFuzzer fuzz targets (requires clang + -fsanitize=fuzzer)" OFF)

if(ANYHOW_BUILD_FUZZ)
  add_executable(anyhow-fuzz fuzz_anyhow.cpp)
  target_link_libraries(anyhow-fuzz PRIVATE anyhow::anyhow)
  target_compile_features(anyhow-fuzz PRIVATE cxx_std_20)
  target_compile_definitions(anyhow-fuzz PRIVATE ANYHOW_SHORT_MACROS)
  target_compile_options(anyhow-fuzz
                         PRIVATE -fsanitize=fuzzer,address,undefined)
  target_link_options(anyhow-fuzz PRIVATE -fsanitize=fuzzer,address,undefined)
endif()
