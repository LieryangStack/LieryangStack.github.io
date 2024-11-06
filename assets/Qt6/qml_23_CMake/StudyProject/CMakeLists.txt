
cmake_minimum_required(VERSION 3.21.1)

# 是否链接跟踪调试、分析应用程序性能相关的库
option(LINK_INSIGHT "Link Qt Insight Tracker library" ON)

# 是否链接Qt Designer Studio定义的qml组件库
option(BUILD_QDS_COMPONENTS "Build design studio components" ON)

project(StudyProjectApp LANGUAGES CXX)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(QT_QML_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/qml)
set(QML_IMPORT_PATH ${QT_QML_OUTPUT_DIRECTORY}
    CACHE STRING "Import paths for Qt Creator's code model"
    FORCE
)

find_package(Qt6 6.7 REQUIRED COMPONENTS Core Gui Qml Quick)
qt_standard_project_setup()


qt_add_executable(${CMAKE_PROJECT_NAME})
qt_add_resources(${CMAKE_PROJECT_NAME} "configuration"
    PREFIX "/"
    FILES qtquickcontrols2.conf)

# 引入 qds.cmake 文件（该cmake文件会导入StudyProject StudyProjectContent App子目录）
include(qds)

# 调试、分析性能相关的库
if (BUILD_QDS_COMPONENTS)
    include(qmlcomponents OPTIONAL)
endif()

# Qt Designer Studio相关qml组件库
if (LINK_INSIGHT)
    include(insight OPTIONAL)
endif ()

include(GNUInstallDirs)
install(TARGETS ${CMAKE_PROJECT_NAME}
  BUNDLE DESTINATION .
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
