cmake_minimum_required(VERSION 3.20) 
project(bulk)

add_library(bulk_lib parser.cpp printer.cpp state.cpp)
set_target_properties(bulk_lib PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
)
target_include_directories(bulk_lib PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")


set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/../cmake")
include(testing)
run_tests(bulk_lib)
