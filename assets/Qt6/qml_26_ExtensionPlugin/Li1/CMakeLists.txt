cmake_minimum_required(VERSION 3.16)

project(Li1 VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(QT_QML_OUTPUT_DIRECTORY  ${CMAKE_BINARY_DIR})

find_package(Qt6 6.8 COMPONENTS Quick REQUIRED)

qt_add_library(Li1 SHARED)
qt_add_qml_module(Li1
    URI Li1
    OUTPUT_DIRECTORY imports/TimeExample
    VERSION 1.0
    QML_FILES MyItemControls.qml
    SOURCES myitem.cpp myitem.h
)

# set_target_properties(Li1 PROPERTIES
#     MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
#     MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
#     MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
#     MACOSX_BUNDLE TRUE
#     WIN32_EXECUTABLE TRUE
# )

# target_compile_definitions(Li1
#     PRIVATE $<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:QT_QML_DEBUG>)
target_link_libraries(Li1
    PRIVATE Qt6::Quick)

target_include_directories(Li1 PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

