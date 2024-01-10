cmake_minimum_required(VERSION 3.10)

include(CTest)

enable_testing()

include(GoogleTest)
find_package(GTest)

add_executable(tests tests.cpp)
target_compile_options(tests PRIVATE -Wall -Wextra -pedantic -Werror)
target_include_directories(tests PUBLIC "${PROJECT_BINARY_DIR}")
target_link_libraries(tests GTest::gtest_main)

gtest_discover_tests(tests)
