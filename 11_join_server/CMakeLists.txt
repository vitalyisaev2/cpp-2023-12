cmake_minimum_required(VERSION 3.20) 
project(join_server)

enable_testing()
# add_subdirectory(bulk)
# add_subdirectory(utils)

add_library(database STATIC table.cpp database.cpp parser.cpp) # No much sense to use SHARED in this task
set_target_properties(database PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
)
target_include_directories(database PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
# target_link_libraries(async PRIVATE bulk_lib)
# install(TARGETS async LIBRARY DESTINATION lib PUBLIC_HEADER DESTINATION include/async)

add_executable(join_server server.cpp)
target_compile_options(join_server PRIVATE -Wall -Wextra -pedantic -Werror)
target_include_directories(join_server PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
target_link_libraries(join_server PRIVATE database)
set_target_properties(join_server PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
)
# install(TARGETS join_server DESTINATION bin)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/../cmake")
# include(package)
# add_package("join_server" "10")
include(testing)
run_tests(database)
