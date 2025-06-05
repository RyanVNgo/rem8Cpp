cmake_minimum_required(VERSION 3.26)
project(rem8CppTests) 

# Build Configuration Stuff
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD              20)
set(CMAKE_CXX_STANDARD_REQUIRED   true)
set(CMAKE_CXX_EXTENSIONS           off)
set(CMAKE_CXX_FLAGS "-O0 -Wall -Wextra ")

# Testing Stuff
include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG 52eb810
)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(
  test_emulator
  test_emulator.cpp
  ${CMAKE_SOURCE_DIR}/../src/emulator.cpp
)

include_directories(${CMAKE_SOURCE_DIR}/../src/)

target_link_libraries(test_emulator
  PRIVATE
  GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(test_emulator)

