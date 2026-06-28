
add_library(imgui STATIC)
target_sources(imgui
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
                    ${CMAKE_CURRENT_SOURCE_DIR}/../../external
                FILES
                    ${CMAKE_CURRENT_SOURCE_DIR}/../../external/imgui/imconfig.h
                    ${CMAKE_CURRENT_SOURCE_DIR}/../../external/imgui/imgui.h
                    ${CMAKE_CURRENT_SOURCE_DIR}/../../external/imgui/imgui_internal.h
                    ${CMAKE_CURRENT_SOURCE_DIR}/../../external/imgui/imstb_rectpack.h
                    ${CMAKE_CURRENT_SOURCE_DIR}/../../external/imgui/imstb_textedit.h
                    ${CMAKE_CURRENT_SOURCE_DIR}/../../external/imgui/imstb_truetype.h
                    ${CMAKE_CURRENT_SOURCE_DIR}/../../external/imgui/backend/imgui_impl_glfw.h
                    ${CMAKE_CURRENT_SOURCE_DIR}/../../external/imgui/backend/imgui_impl_opengl3.h
                    ${CMAKE_CURRENT_SOURCE_DIR}/../../external/imgui/backend/imgui_impl_opengl3_loader.h
)
target_link_libraries(imgui
    PRIVATE
    GLEW::GLEW
    glfw
)

target_compile_features(imgui PUBLIC cxx_std_20)

