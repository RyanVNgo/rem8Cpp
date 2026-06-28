cmake_minimum_required(VERSION 3.28)

project(rem8C++
    VERSION 1.1.1
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD              20)
set(CMAKE_CXX_STANDARD_REQUIRED     ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

include_directories(
    "${CMAKE_CURRENT_SOURCE_DIR}/external/imgui/"
    "${CMAKE_CURRENT_SOURCE_DIR}/external/imgui/backend"
)

find_package(glfw3 CONFIG REQUIRED)
find_package(GLEW REQUIRED)

add_subdirectory(external/imgui)
add_subdirectory(src/rem8Cpp)

option(BUILD_TESTS "Build the test suite" OFF)
if(BUILD_TESTS)
    add_subdirectory(tests)
endif()

