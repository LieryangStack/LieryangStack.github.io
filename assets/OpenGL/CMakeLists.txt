# cmake最低版本要求3.10
cmake_minimum_required(VERSION 3.10)

# 该项目名称为OpenGLStudy，该项目编程语言C++
project(OpenGLStudy VERSION 1.0 LANGUAGES CXX)

find_package(PkgConfig REQUIRED)

pkg_search_module(PKG_GSTREAMER REQUIRED glfw3)

add_executable(01_win 2024040801/01_create_window.cpp glad/src/glad.c)

target_include_directories(01_win PUBLIC
                           "${PROJECT_SOURCE_DIR}/glad/include"
                           )

# find_package(X11 REQUIRED)

# message(${X11_INCLUDE_DIRS})

# message(${PKG_GSTREAMER_INCLUDE_DIRS})

# message(${PKG_GSTREAMER_LIBRARIES})
