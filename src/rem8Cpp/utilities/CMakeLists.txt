
add_library(rem8Cpp.utilities STATIC)
add_library(rem8Cpp::utilities ALIAS rem8Cpp.utilities)
target_sources(rem8Cpp.utilities
    PRIVATE
        file.cpp
    PUBLIC
        FILE_SET HEADERS
            BASE_DIRS
                ${CMAKE_CURRENT_SOURCE_DIR}/../../rem8Cpp
            FILES
                ${CMAKE_CURRENT_SOURCE_DIR}/../../rem8Cpp/utilities/file.h
                ${CMAKE_CURRENT_SOURCE_DIR}/../../rem8Cpp/utilities/instrumentor.h
)

target_compile_features(rem8Cpp.utilities PUBLIC cxx_std_20)

