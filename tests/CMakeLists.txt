
enable_testing()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/tests)

include(FetchContent)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY  https://github.com/google/googletest.git
    GIT_TAG         52eb810
)
FetchContent_MakeAvailable(googletest)

add_executable(test_emulator
    test_emulator.cpp
)
target_link_libraries(test_emulator
    PRIVATE
        rem8Cpp::emulator
        GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(test_emulator)

