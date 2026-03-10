cmake_minimum_required(VERSION 3.10)

project(Signal
  LANGUAGES CXX
)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(Threads REQUIRED)

# Auto download googletest
include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        52eb8108c5bdec04579160ae17225d66034bd723 # v1.17.0
)
FetchContent_MakeAvailable(googletest)

add_executable(testSignal
  testSignal.cc
)

target_include_directories(testSignal
  PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/googletest/googletest/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/googletest/googletest"
)

target_compile_options(testSignal
  PRIVATE
  "-Wall" "-Wextra" "-g" "-O0" "-fsanitize=address,undefined"
)

target_compile_features(testSignal
  PUBLIC
    cxx_std_17
)

set_target_properties(testSignal
  PROPERTIES
    CXX_EXTENSIONS OFF
    LINK_FLAGS "-fsanitize=address,undefined"
)

target_link_libraries(testSignal
  PRIVATE
    GTest::gtest_main
    Threads::Threads
)

include(GoogleTest)
gtest_discover_tests(testSignal)
