cmake_minimum_required(VERSION 3.20) 
project(bulk)

add_library(bulk_lib parser.cpp printer.cpp state.cpp)
set_target_properties(bulk_lib PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
)
target_include_directories(bulk_lib PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")

add_executable(bulk main.cpp)
target_compile_options(bulk PRIVATE -Wall -Wextra -pedantic -Werror)
target_include_directories(bulk PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
target_link_libraries(bulk PRIVATE bulk_lib)
set_target_properties(bulk PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
)

install(TARGETS bulk DESTINATION bin)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/../cmake")
include(package)
add_package("bulk" "07")
include(testing)
run_tests(bulk_lib)
