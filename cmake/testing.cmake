cmake_minimum_required(VERSION 3.20)

macro(run_tests library)
    include(CTest)
    include(GoogleTest)
    find_package(GTest)

    set(path "${CMAKE_CURRENT_SOURCE_DIR}")
    cmake_path(GET path FILENAME DIRNAME)
    set(TESTS_BINARY_NAME "${DIRNAME}_tests")

    add_executable("${TESTS_BINARY_NAME}" tests.cpp)
    # target_compile_options("${TESTS_BINARY_NAME}" PRIVATE -Wall -Wextra -pedantic -Werror)
    target_include_directories("${TESTS_BINARY_NAME}" PUBLIC "${PROJECT_BINARY_DIR}")
    target_link_libraries("${TESTS_BINARY_NAME}" GTest::gtest_main "${library}" Boost::program_options Boost::system Boost::filesystem)
    set_target_properties("${TESTS_BINARY_NAME}" PROPERTIES
        CXX_STANDARD 20
        CXX_STANDARD_REQUIRED ON
    )

    gtest_discover_tests("${TESTS_BINARY_NAME}")
endmacro()
