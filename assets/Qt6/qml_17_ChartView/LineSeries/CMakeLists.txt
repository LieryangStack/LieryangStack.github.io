cmake_minimum_required(VERSION 3.16)

project(LineSeries VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 6.5 REQUIRED COMPONENTS Quick)
find_package(Qt6 REQUIRED COMPONENTS Widgets)
find_package(Qt6 REQUIRED COMPONENTS Charts)

qt_standard_project_setup(REQUIRES 6.5)

qt_add_executable(appLineSeries
    main.cpp
)

qt_add_qml_module(appLineSeries
    URI LineSeries
    VERSION 1.0
    QML_FILES Main.qml
    QML_FILES SpeedsList.qml
    QML_FILES
    QML_FILES MyLineSeries.qml
)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
set_target_properties(appLineSeries PROPERTIES
#    MACOSX_BUNDLE_GUI_IDENTIFIER com.example.appLineSeries
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

target_link_libraries(appLineSeries
    PRIVATE Qt6::Quick
)

target_link_libraries(appLineSeries
  PRIVATE Qt6::Widgets
)

target_link_libraries(appLineSeries
  PRIVATE Qt6::Charts
)

include(GNUInstallDirs)


install(TARGETS appLineSeries
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
