cmake_minimum_required(VERSION 3.20) 
project(bayan)

find_package(Boost REQUIRED COMPONENTS program_options system filesystem REQUIRED)

if(NOT Boost_FOUND)
    message(FATAL_ERROR "Boost Not found")
endif()

# set(Boost_USE_STATIC_LIBS        ON)  # only find static libs
# set(Boost_USE_DEBUG_LIBS        OFF)  # ignore debug libs and
# set(Boost_USE_RELEASE_LIBS       ON)  # only find release libs
set(Boost_USE_MULTITHREADED      ON)
set(Boost_USE_STATIC_RUNTIME    OFF)


add_library(bayan_lib 
    checksum_computer.cpp
    file_checker.cpp
)
set_target_properties(bayan_lib PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
)
target_include_directories(bayan_lib PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")
target_link_libraries(bayan_lib Boost::program_options Boost::system Boost::filesystem)

add_executable(bayan main.cpp)
target_compile_options(bayan PRIVATE -Wall -Wextra -pedantic -Werror)
target_include_directories(bayan PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
target_link_libraries(bayan PRIVATE bayan_lib)
set_target_properties(bayan PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
)

install(TARGETS bayan DESTINATION bin)

include(CTest)
include(GoogleTest)
find_package(GTest)

set(TESTS_BINARY_NAME "bayan_tests")

add_executable("${TESTS_BINARY_NAME}" tests.cpp)
# target_compile_options("${TESTS_BINARY_NAME}" PRIVATE -Wall -Wextra -pedantic -Werror)
target_include_directories("${TESTS_BINARY_NAME}" PUBLIC "${PROJECT_BINARY_DIR}")
target_link_libraries("${TESTS_BINARY_NAME}" GTest::gtest_main "${library}" Boost::system Boost::filesystem)
set_target_properties("${TESTS_BINARY_NAME}" PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
)

gtest_discover_tests("${TESTS_BINARY_NAME}")

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/../cmake")
# include(package)
# add_package("bayan" "08")
# include(testing)
# run_tests(bayan_lib)
