
add_library(rem8Cpp.widgets STATIC)
add_library(rem8Cpp::widgets ALIAS rem8Cpp.widgets)
target_sources(rem8Cpp.widgets
    PRIVATE
        widgets.cpp
        control_panel.cpp
        file_explorer.cpp
    PUBLIC
        FILE_SET HEADERS
            BASE_DIRS
                ${CMAKE_CURRENT_SOURCE_DIR}/../../rem8Cpp
            FILES
                ${CMAKE_CURRENT_SOURCE_DIR}/../../rem8Cpp/widgets/widgets.h
                ${CMAKE_CURRENT_SOURCE_DIR}/../../rem8Cpp/widgets/control_panel.h
                ${CMAKE_CURRENT_SOURCE_DIR}/../../rem8Cpp/widgets/file_explorer.h
)
target_link_libraries(rem8Cpp.widgets
    PRIVATE
    GLEW::GLEW
    glfw
)

target_compile_features(rem8Cpp.widgets PUBLIC cxx_std_20)

