cmake_minimum_required(VERSION 3.10)

# 设定项目名称和版本号
project(FFmpegStudy VERSION 1.0
                    DESCRIPTION "学习FFmpeg音视频")

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

# 查找FFmpeg库
find_package(PkgConfig REQUIRED)
pkg_check_modules(FFMPEG REQUIRED libavformat libavcodec libavutil)

# 添加头文件路径
include_directories(${FFMPEG_INCLUDE_DIRS})

# 链接FFmpeg库
link_libraries(${FFMPEG_LIBRARIES})

add_subdirectory(03_FFmpeg)