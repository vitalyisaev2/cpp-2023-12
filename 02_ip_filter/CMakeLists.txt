cmake_minimum_required(VERSION 3.20) 
project(ip_filter)

add_library(ip_filter_lib address.cpp addresses.cpp)
set_target_properties(ip_filter_lib PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)
target_include_directories(ip_filter_lib PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")

add_executable(ip_filter main.cpp)
target_compile_options(ip_filter PRIVATE -Wall -Wextra -pedantic -Werror)
target_include_directories(ip_filter PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
target_link_libraries(ip_filter PRIVATE ip_filter_lib)
install(TARGETS ip_filter DESTINATION bin)


set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/../cmake")
include(package)
add_package("ip_filter" "02")
include(testing)
run_tests(ip_filter_lib)