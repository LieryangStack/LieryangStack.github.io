include(C:/Users/lieryang/Desktop/LieryangStack.github.io/assets/Qt6/qml_20_models/abstractitemmodel/build/Desktop_Qt_6_7_2_MinGW_64_bit-Debug/.qt/QtDeploySupport.cmake)
include("${CMAKE_CURRENT_LIST_DIR}/abstractitemmodelexample-plugins.cmake" OPTIONAL)
set(__QT_DEPLOY_ALL_MODULES_FOUND_VIA_FIND_PACKAGE "ZlibPrivate;EntryPointPrivate;Core;Gui;QmlIntegration;QmlBuiltins;Network;Qml;QmlModels;OpenGL;Quick")

qt6_deploy_qml_imports(TARGET abstractitemmodelexample PLUGINS_FOUND plugins_found)
qt6_deploy_runtime_dependencies(
    EXECUTABLE C:/Users/lieryang/Desktop/LieryangStack.github.io/assets/Qt6/qml_20_models/abstractitemmodel/build/Desktop_Qt_6_7_2_MinGW_64_bit-Debug/abstractitemmodelexample.exe
    ADDITIONAL_MODULES ${plugins_found}
    GENERATE_QT_CONF
)