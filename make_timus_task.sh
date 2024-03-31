#!/usr/bin/env bash

TASK_DIR="./timus/$1"

if [ -d "$TASK_DIR" ]; then
  echo "$TASK_DIR already exists.";
  exit 1;
fi

mkdir -p "$TASK_DIR"

cat << EOF > "$TASK_DIR"/CMakeLists.txt
cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 17)

add_executable(timus$1 main.cpp)
EOF

cat << EOF > "$TASK_DIR"/main.cpp
#include <iostream>

int main() {
    return 0;
}
EOF

rm -rf ./build/
mkdir ./build/
cd ./build
CXX=/usr/bin/clang++ cmake -DCMAKE_BUILD_TYPE=Debug ../timus/$1
