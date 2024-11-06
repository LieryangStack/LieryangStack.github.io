cmake_minimum_required(VERSION 3.16)

project(Test VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(QT_QML_OUTPUT_DIRECTORY  ${CMAKE_BINARY_DIR})

find_package(Qt6 6.2 COMPONENTS Quick REQUIRED)

qt_add_library(Test STATIC)
qt_add_qml_module(Test
    URI Test
    VERSION 1.0
    QML_FILES MyItemControls.qml # 这是 qml 编写的组件对象 MyItemControls（描述文件是qmldir）
    SOURCES myitem.cpp myitem.h # 这是C++编写的组件对象 MyItem（描述文件是Test.qmltypes）
)

set_target_properties(Test PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

target_compile_definitions(Test
    PRIVATE $<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:QT_QML_DEBUG>)
target_link_libraries(Test
    PRIVATE Qt6::Quick)

target_include_directories(Test PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# Example Project
qt_add_executable(ExampleProject example/example.cpp)
qt_add_qml_module(ExampleProject
    URI ExampleProjectApp
    VERSION 1.0
    QML_FILES example/example.qml
)

target_link_libraries(ExampleProject PRIVATE Qt6::Quick Testplugin)
target_compile_definitions(ExampleProject PRIVATE $<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:QT_QML_DEBUG>)
