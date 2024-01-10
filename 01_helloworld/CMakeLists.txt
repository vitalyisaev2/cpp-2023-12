cmake_minimum_required(VERSION 3.9)

project(helloworld)

set(BUILD_NUMBER 0 CACHE STRING "Build number")
configure_file(version.h.in version.h)

add_executable(helloworld_cli main.cpp)
target_compile_options(helloworld_cli PRIVATE -Wall -Wextra -pedantic -Werror)
target_include_directories(helloworld_cli PUBLIC "${PROJECT_BINARY_DIR}")
install(TARGETS helloworld_cli DESTINATION bin)

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(package)
include(testing)
