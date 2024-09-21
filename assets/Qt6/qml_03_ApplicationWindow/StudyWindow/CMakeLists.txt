# cmake最低版本要求 3.16
cmake_minimum_required(VERSION 3.16)

# 项目名称StudyWindow
# 版本 0.1
# 编程语言 CXX
project(StudyWindow VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD_REQUIRED ON)


# 先会查找 Qt6Config.camke 或者 FindQt6.cmake
# 然后会在该文件中，查找 Quick 组件
find_package(Qt6 6.5 REQUIRED COMPONENTS Quick)

qt_standard_project_setup(REQUIRES 6.5)

qt_add_executable(appStudyWindow
    main.cpp
)

qt_add_qml_module(appStudyWindow
    URI StudyWindow
    VERSION 1.0
    QML_FILES Test.qml
    QML_FILES WindowFrameLess.qml
)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
set_target_properties(appStudyWindow PROPERTIES
#    MACOSX_BUNDLE_GUI_IDENTIFIER com.example.appStudyWindow
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

target_link_libraries(appStudyWindow
    PRIVATE Qt6::Quick
)

include(GNUInstallDirs)
install(TARGETS appStudyWindow
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
