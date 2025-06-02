cmake_minimum_required(VERSION 3.26)
include(CheckLanguage)

project(rem8C++ VERSION 0.1.0 LANGUAGES CXX)

add_executable(${PROJECT_NAME})

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD              20)
set(CMAKE_CXX_COMPILER             g++)
set(CMAKE_CXX_STANDARD_REQUIRED   true)
set(CMAKE_CXX_EXTENSIONS           off)
set(CMAKE_CXX_FLAGS "-O0 -Wall -Wextra -D_REENTRANT")
set(CMAKE_CXX_FLAGS_DEBUG_INIT "-O0 -Wall -Wextra -D_REENTRANT") # -DCMAKE_BUILD_TYPE=Debug
set(CMAKE_CXX_FLAGS_RELEASE_INIT "-O3 -Wall -Wextra -D_REENTRANT") # -DCMAKE_BUILD_TYPE=Release

file(GLOB_RECURSE CPP_SOURCES     "src/*.cpp")
set_source_files_properties(${CPP_SOURCES} PROPERTIES LANGUAGE CXX)
target_sources(${PROJECT_NAME} PRIVATE ${CPP_SOURCES})

include_directories(${CMAKE_SOURCE_DIR}/src/)
include_directories(${CMAKE_SOURCE_DIR}/src/imgui)
include_directories(${CMAKE_SOURCE_DIR}/src/imgui/backend)

target_include_directories(${PROJECT_NAME} PRIVATE src/imgui src/imgui/backends)
target_link_libraries(${PROJECT_NAME} PRIVATE SDL2 GL)

option(ENABLE_PROFILING "Enable profiling features" OFF)
if (ENABLE_PROFILING)
  message(STATUS "  Profiling Enabled")
  add_compile_definitions(PROFILING)
else()
  message(STATUS "  Profiling Disabled")
endif()

