# cmake最低版本要求3.10
cmake_minimum_required(VERSION 3.10)

# 该项目名称为OpenGLStudy，该项目编程语言C++
project(OpenGLStudy VERSION 1.0 LANGUAGES C CXX)

# 使用PkgConfig组件
find_package(PkgConfig REQUIRED)

# 通过pkg-config查找 glfw3 
pkg_search_module(PKG_GLFW REQUIRED glfw3)


add_library(glad STATIC glad/glad.c)

target_include_directories(glad PUBLIC
                           "${PROJECT_SOURCE_DIR}"
                          )

# add_executable(01_win 2024040801/01_create_window.cpp)

# find_library(GL_LIB glfw3 "${PROJECT_SOURCE_DIR}/lib")

include_directories(${PROJECT_SOURCE_DIR})

link_libraries(${PKG_GLFW_LIBRARIES} glad)

add_subdirectory(2024040801)

# target_link_libraries(01_win ${PKG_GLFW_LIBRARIES} glad)

# target_include_directories(01_win PUBLIC
#                            ${PROJECT_SOURCE_DIR}
#                            ${PKG_GLFW_INCLUDE_DIRS}
#                            )

# message( "PKG_GLFW_LIBRARIES = " ${PKG_GLFW_LIBRARIES} )
# message( "GL_LIB =  " ${GL_LIB} )

# find_package(X11 REQUIRED)

# message(${X11_INCLUDE_DIRS})

# message(${PKG_GSTREAMER_INCLUDE_DIRS})

# message(${PKG_GSTREAMER_LIBRARIES})
