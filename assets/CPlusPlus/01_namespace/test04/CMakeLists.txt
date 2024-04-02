cmake_minimum_required(VERSION 3.10)

# 设定项目名称和版本号
project(Tutorial VERSION 1.0)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

add_executable(main main.cpp h1.cpp)

target_include_directories(main PUBLIC
                           "${PROJECT_BINARY_DIR}"
                           )