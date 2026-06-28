cmake_minimum_required(VERSION 3.20)

project(rem8C++ VERSION 1.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD              20)
set(CMAKE_CXX_STANDARD_REQUIRED     ON)

add_executable(${PROJECT_NAME})

target_compile_options(${PROJECT_NAME}
    PRIVATE
    -Wall
    -Wextra
    -Wpedantic
)

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

