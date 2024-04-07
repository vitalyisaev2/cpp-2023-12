cmake_minimum_required(VERSION 3.20) 
project(bayan)

find_package(Boost REQUIRED COMPONENTS program_options system filesystem log REQUIRED)

if(NOT Boost_FOUND)
    message(FATAL_ERROR "Boost Not found")
endif()

# set(Boost_USE_STATIC_LIBS        ON)  # only find static libs
# set(Boost_USE_DEBUG_LIBS        OFF)  # ignore debug libs and
# set(Boost_USE_RELEASE_LIBS       ON)  # only find release libs
set(Boost_USE_MULTITHREADED      ON)
set(Boost_USE_STATIC_RUNTIME    OFF)


add_library(bayan_lib 
    block_checksum_storage.cpp
    checksum_computer.cpp
    file_block_checksum_computer.cpp
)
set_target_properties(bayan_lib PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
)
target_include_directories(bayan_lib PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")
target_link_libraries(bayan_lib Boost::program_options Boost::system Boost::filesystem Boost::log)

add_executable(bayan main.cpp)
target_compile_options(bayan PRIVATE -Wall -Wextra -pedantic -Werror)
target_include_directories(bayan PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
target_link_libraries(bayan PRIVATE bayan_lib)
set_target_properties(bayan PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
)

install(TARGETS bayan DESTINATION bin)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/../cmake")
# include(package)
# add_package("bayan" "08")
include(testing)
run_tests(bayan_lib)
