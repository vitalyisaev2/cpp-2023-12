cmake_minimum_required(VERSION 3.20) 
project(bulk_server)

set(Boost_USE_STATIC_LIBS        ON)  # only find static libs
# set(Boost_USE_DEBUG_LIBS        OFF)  # ignore debug libs and
# set(Boost_USE_RELEASE_LIBS       ON)  # only find release libs
set(Boost_USE_MULTITHREADED      ON)
set(Boost_USE_STATIC_RUNTIME    OFF)

#find_package(Boost REQUIRED COMPONENTS program_options system filesystem log REQUIRED)
#find_package(Boost REQUIRED COMPONENTS asio REQUIRED)
#
#if(NOT Boost_FOUND)
#    message(FATAL_ERROR "Boost Not found")
#endif()

# add_library(bayan_lib 
#     block_checksum_storage.cpp
#     checksum_computer.cpp
#     file_block_checksum_computer.cpp
#     file_crawler.cpp
# )
# set_target_properties(bayan_lib PROPERTIES
#     CXX_STANDARD 20
#     CXX_STANDARD_REQUIRED ON
# )
# target_include_directories(bayan_lib PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")
# target_link_libraries(
#     bayan_lib
#     Boost::program_options
#     Boost::system
#     Boost::filesystem
#     Boost::log
#     OpenSSL::Crypto
# )

add_executable(bulk_server echo_server.cpp)
target_compile_options(bulk_server PRIVATE -Wall -Wextra -pedantic -Werror)
target_include_directories(bulk_server PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
# target_link_libraries(bayan PRIVATE bayan_lib)
set_target_properties(bulk_server PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
)

install(TARGETS bulk_server DESTINATION bin)

# set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/../cmake")
# include(package)
# add_package("bayan" "08")
# include(testing)
# set(LIBRARY_NAMES_LIST 
#     bayan_lib
#     Boost::program_options
#     Boost::system
#     Boost::filesystem
#     Boost::log
#     OpenSSL::Crypto
# )
# run_tests("${LIBRARY_NAMES_LIST}")
