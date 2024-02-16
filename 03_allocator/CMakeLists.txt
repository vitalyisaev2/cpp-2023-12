cmake_minimum_required(VERSION 3.20) 
project(allocator)

add_library(allocator_lib INTERFACE)
set_target_properties(allocator_lib PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)
target_include_directories(allocator_lib INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}")


add_executable(allocator main.cpp)
target_compile_options(allocator PRIVATE -Wall -Wextra -pedantic -Werror)
target_link_libraries(allocator PRIVATE allocator_lib)

install(TARGETS allocator DESTINATION bin)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/../cmake")
include(package)
add_package("allocator" "03")
include(testing)
run_tests(allocator_lib)
