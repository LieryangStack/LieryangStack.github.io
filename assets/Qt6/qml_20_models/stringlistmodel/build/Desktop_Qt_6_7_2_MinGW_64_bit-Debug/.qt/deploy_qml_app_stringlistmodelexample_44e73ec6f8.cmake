include(C:/Users/Lieryang/Desktop/LieryangStack.github.io/assets/Qt6/qml_20_models/stringlistmodel/build/Desktop_Qt_6_7_2_MinGW_64_bit-Debug/.qt/QtDeploySupport.cmake)
include("${CMAKE_CURRENT_LIST_DIR}/stringlistmodelexample-plugins.cmake" OPTIONAL)
set(__QT_DEPLOY_ALL_MODULES_FOUND_VIA_FIND_PACKAGE "ZlibPrivate;EntryPointPrivate;Core;Gui;QmlIntegration;QmlBuiltins;Network;Qml;QmlModels;OpenGL;Quick")

qt6_deploy_qml_imports(TARGET stringlistmodelexample PLUGINS_FOUND plugins_found)
qt6_deploy_runtime_dependencies(
    EXECUTABLE C:/Users/Lieryang/Desktop/LieryangStack.github.io/assets/Qt6/qml_20_models/stringlistmodel/build/Desktop_Qt_6_7_2_MinGW_64_bit-Debug/stringlistmodelexample.exe
    ADDITIONAL_MODULES ${plugins_found}
    GENERATE_QT_CONF
)