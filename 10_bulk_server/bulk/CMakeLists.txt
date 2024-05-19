cmake_minimum_required(VERSION 3.20) 
project(bulk)

include_directories(${CMAKE_SOURCE_DIR}/utils)
link_directories(${CMAKE_SOURCE_DIR}/utils)

add_library(bulk_lib accumulator.cpp accumulator_factory.cpp parser.cpp printer.cpp state.cpp)
set_target_properties(bulk_lib PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
)
target_compile_options(bulk_lib PRIVATE -Wall -Wextra -pedantic -Werror -fPIC)
target_include_directories(bulk_lib PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}" utils_lib)
target_link_libraries(bulk_lib utils_lib)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/../cmake")
include(testing)
run_tests(bulk_lib)
