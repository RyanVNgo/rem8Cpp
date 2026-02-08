cmake_minimum_required(VERSION 3.20)

if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    set(VCPKG_ROOT "${CMAKE_SOURCE_DIR}/external/vcpkg")
    if(EXISTS "${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake")
        set(CMAKE_TOOLCHAIN_FILE
            "${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake"
            CACHE STRING "Vcpkg toolchain file")
    else()
        message(FATAL_ERROR
            "vcpkg not found at ${VCPKG_ROOT}.\n"
            "Did you forget to run `git submodule update --init --recursive`?")
    endif()
endif()

project(rem8C++ VERSION 1.1.0 LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD              20)
set(CMAKE_CXX_COMPILER             g++)
set(CMAKE_CXX_STANDARD_REQUIRED     ON)
add_compile_options(-Wall -Wextra -Wpedantic)

add_executable(${PROJECT_NAME})

include_directories(
  ${CMAKE_SOURCE_DIR}/src/

  ${CMAKE_SOURCE_DIR}/src/imgui
  ${CMAKE_SOURCE_DIR}/src/imgui/backend
)

file(GLOB IMGUI_SOURCES           "${CMAKE_SOURCE_DIR}/src/imgui/*.cpp")
file(GLOB IMGUI_BACKEND_SOURCES   "${CMAKE_SOURCE_DIR}/src/imgui/backend/*.cpp")

target_sources(
  ${PROJECT_NAME} 
  PRIVATE 

  ${CMAKE_SOURCE_DIR}/src/main.cpp
  ${CMAKE_SOURCE_DIR}/src/emulator.cpp

  ${CMAKE_SOURCE_DIR}/src/user_interface/window.cpp
  ${CMAKE_SOURCE_DIR}/src/user_interface/graphics.cpp

  ${CMAKE_SOURCE_DIR}/src/widgets/widgets.cpp
  ${CMAKE_SOURCE_DIR}/src/widgets/control_panel.cpp
  ${CMAKE_SOURCE_DIR}/src/widgets/file_explorer.cpp

  ${CMAKE_SOURCE_DIR}/src/utilities/file.cpp

  ${IMGUI_SOURCES}
  ${IMGUI_BACKEND_SOURCES}
)

find_package(glfw3 CONFIG REQUIRED)
find_package(GLEW REQUIRED)

target_link_libraries(${PROJECT_NAME}
  PRIVATE
  glfw
  GLEW::GLEW
)

# option(ENABLE_PROFILING "Enable profiling features" OFF)
# if (ENABLE_PROFILING)
#   message(STATUS "  Profiling Enabled")
#   add_compile_definitions(PROFILING)
# else()
#   message(STATUS "  Profiling Disabled")
# endif()

