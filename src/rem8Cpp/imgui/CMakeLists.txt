
add_library(rem8Cpp.imgui STATIC)
add_library(rem8Cpp::imgui ALIAS rem8Cpp.imgui)
target_sources(rem8Cpp.imgui
    PRIVATE
        imgui.cpp
        imgui_demo.cpp
        imgui_draw.cpp
        imgui_tables.cpp
        imgui_widgets.cpp
        backend/imgui_impl_glfw.cpp
        backend/imgui_impl_opengl3.cpp
    PUBLIC
        FILE_SET HEADERS
            BASE_DIRS
                ${CMAKE_CURRENT_SOURCE_DIR}/../../rem8Cpp
            FILES
                ${CMAKE_CURRENT_SOURCE_DIR}/../../rem8Cpp/imgui/imconfig.h
                ${CMAKE_CURRENT_SOURCE_DIR}/../../rem8Cpp/imgui/imgui.h
                ${CMAKE_CURRENT_SOURCE_DIR}/../../rem8Cpp/imgui/imgui_internal.h
                ${CMAKE_CURRENT_SOURCE_DIR}/../../rem8Cpp/imgui/imstb_rectpack.h
                ${CMAKE_CURRENT_SOURCE_DIR}/../../rem8Cpp/imgui/imstb_textedit.h
                ${CMAKE_CURRENT_SOURCE_DIR}/../../rem8Cpp/imgui/imstb_truetype.h
                ${CMAKE_CURRENT_SOURCE_DIR}/../../rem8Cpp/imgui/backend/imgui_impl_glfw.h
                ${CMAKE_CURRENT_SOURCE_DIR}/../../rem8Cpp/imgui/backend/imgui_impl_opengl3.h
                ${CMAKE_CURRENT_SOURCE_DIR}/../../rem8Cpp/imgui/backend/imgui_impl_opengl3_loader.h
)
target_link_libraries(rem8Cpp.imgui
    PRIVATE
    GLEW::GLEW
    glfw
)

target_compile_features(rem8Cpp.imgui PUBLIC cxx_std_20)

