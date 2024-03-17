cmake_minimum_required(VERSION 3.20) 
project(matrix)

# set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address  -fno-omit-frame-pointer -g -O0")

add_library(matrix_lib INTERFACE)
set_target_properties(matrix_lib PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
)
target_include_directories(matrix_lib INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}")

add_executable(matrix main.cpp)
target_compile_options(matrix PRIVATE -Wall -Wextra -pedantic -Werror)
target_link_libraries(matrix PRIVATE matrix_lib)
set_target_properties(matrix PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
)

install(TARGETS matrix DESTINATION bin)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/../cmake")
include(package)
add_package("matrix" "06")
include(testing)
run_tests(matrix_lib)
