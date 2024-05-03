cmake_minimum_required(VERSION 3.20) 
project(utils)

add_library(utils_lib INTERFACE)
set_target_properties(utils_lib PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
)
target_include_directories(utils_lib PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/../cmake")
include(testing)
run_tests(utils_lib)
