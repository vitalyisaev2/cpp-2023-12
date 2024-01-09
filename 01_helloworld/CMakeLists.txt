cmake_minimum_required(VERSION 3.10)

project(helloworld)

set(BUILD_NUMBER 0 CACHE STRING "Build number")

configure_file(version.h.in version.h)
add_executable(helloworld main.cpp)
target_include_directories(helloworld PUBLIC "${PROJECT_BINARY_DIR}")
