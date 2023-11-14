cmake_minimum_required(VERSION 3.10)

project(SerialPortTest VERSION 1.0)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

# add_library(interface STATIC 
#             interface.cpp 
#             WzSerialPort.cpp)


add_executable(main main.c
                    interface.cpp 
                    WzSerialPort.cpp)

# target_link_libraries(main PUBLIC interface)


# TODO 9: Use target_include_directories to include ${PROJECT_BINARY_DIR}
target_include_directories(main PUBLIC
                           "${PROJECT_BINARY_DIR}"
                           )