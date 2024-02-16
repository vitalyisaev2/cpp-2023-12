cmake_minimum_required(VERSION 3.20) 
project(print_ip)

add_library(print_ip_lib INTERFACE)
set_target_properties(print_ip_lib PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)
target_include_directories(print_ip_lib INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}")

add_executable(print_ip main.cpp)
target_compile_options(print_ip PRIVATE -Wall -Wextra -pedantic -Werror)
target_link_libraries(print_ip PRIVATE print_ip_lib)

install(TARGETS print_ip DESTINATION bin)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/../cmake")
include(package)
add_package("print_ip" "04")# 
include(testing)
run_tests(print_ip_lib)
