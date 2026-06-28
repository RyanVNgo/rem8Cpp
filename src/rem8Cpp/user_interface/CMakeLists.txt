
add_library(rem8Cpp.user_interface STATIC)
add_library(rem8Cpp::user_interface ALIAS rem8Cpp.user_interface)
target_sources(rem8Cpp.user_interface
    PRIVATE
        graphics.cpp
        window.cpp
    PUBLIC
        FILE_SET HEADERS
            BASE_DIRS
                ${CMAKE_CURRENT_SOURCE_DIR}/../../rem8Cpp
            FILES
                ${CMAKE_CURRENT_SOURCE_DIR}/../../rem8Cpp/user_interface/graphics.h
                ${CMAKE_CURRENT_SOURCE_DIR}/../../rem8Cpp/user_interface/window.h
)
target_link_libraries(rem8Cpp.user_interface
    PRIVATE
    GLEW::GLEW
)

target_compile_features(rem8Cpp.user_interface PUBLIC cxx_std_20)

