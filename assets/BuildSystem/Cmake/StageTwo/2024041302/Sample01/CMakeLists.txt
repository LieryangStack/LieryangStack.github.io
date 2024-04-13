# cmake最低版本要求3.10
cmake_minimum_required(VERSION 3.10)

# 该项目名称为OpenGLStudy，该项目编程语言C++
project(CMakeStudy VERSION 1.0 
                   DESCRIPTION "学习CMake构建文件编写"
                   HOMEPAGE_URL  "https://lieryangstack.github.io/"
                   LANGUAGES C CXX
                   )

file(GLOB CPP_SOURCES "${PROJECT_SOURCE_DIR}/src/*.cpp")

file(GLOB C_SOURCES "${PROJECT_SOURCE_DIR}/src/*.c")                

message("CPP_SOURCES = " ${CPP_SOURCES})

# 有无引号都可以
message("C_SOURCES = " "${C_SOURCES}")

add_executable(main ${CPP_SOURCES} ${C_SOURCES})
