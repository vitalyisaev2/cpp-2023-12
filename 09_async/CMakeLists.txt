cmake_minimum_required(VERSION 3.20) 
project(async)

# add_library(async parser.cpp printer.cpp state.cpp)
add_library(async async.cpp)
set_target_properties(async PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
)
target_include_directories(async PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")

add_executable(async_example main.cpp)
target_compile_options(async_example PRIVATE -Wall -Wextra -pedantic -Werror)
target_include_directories(async_example PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
target_link_libraries(async_example PRIVATE async)
set_target_properties(async_example PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
)

install(TARGETS async_example DESTINATION bin)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/../cmake")
include(package)
add_package("async" "09")
# include(testing)
# run_tests(async)