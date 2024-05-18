cmake_minimum_required(VERSION 3.20) 
project(bulk_server)

enable_testing()
add_subdirectory(bulk)
add_subdirectory(utils)

add_library(async SHARED async.cpp async.hpp)
set_target_properties(async PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
    PUBLIC_HEADER "async.hpp"
)
target_include_directories(async PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
target_link_libraries(async PRIVATE bulk_lib)
install(TARGETS async LIBRARY DESTINATION lib PUBLIC_HEADER DESTINATION include/async)

add_executable(bulk_server main.cpp)
target_compile_options(bulk_server PRIVATE -Wall -Wextra -pedantic -Werror)
target_include_directories(bulk_server PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
target_link_libraries(bulk_server PRIVATE async)
set_target_properties(bulk_server PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
)

install(TARGETS bulk_server DESTINATION bin)

# set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/../cmake")
# include(package)
# add_package("async" "09")
# include(testing)
# run_tests(async)
