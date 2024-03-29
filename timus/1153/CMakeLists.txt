cmake_minimum_required(VERSION 3.20) 
project(timus1153)

add_executable(timus1153 main.cpp)
set_target_properties(timus1153 PROPERTIES
    CXX_STANDARD 14
    CXX_STANDARD_REQUIRED ON
)

# target_compile_options(timus1153 PRIVATE -Wall -Wextra -pedantic -Werror)
