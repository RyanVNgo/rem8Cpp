
add_subdirectory(widgets)
add_subdirectory(utilities)
add_subdirectory(user_interface)
add_subdirectory(imgui)

add_library(rem8Cpp.emulator STATIC)
add_library(rem8Cpp::emulator ALIAS rem8Cpp.emulator)
target_sources(rem8Cpp.emulator
    PRIVATE
        emulator.cpp
    PUBLIC
        FILE_SET HEADERS
            BASE_DIRS
                ${CMAKE_CURRENT_SOURCE_DIR}/../rem8Cpp
            FILES
                ${CMAKE_CURRENT_SOURCE_DIR}/../rem8Cpp/emulator.h
)

target_compile_features(rem8Cpp.emulator PUBLIC cxx_std_20)

add_executable(rem8Cpp
    main.cpp
)

target_link_libraries(rem8Cpp
    PRIVATE
        rem8Cpp::emulator
        rem8Cpp::widgets
        rem8Cpp::utilities
        rem8Cpp::user_interface
        rem8Cpp::imgui
        glfw
        GLEW::GLEW
)

