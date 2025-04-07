include(C:/ProgramData/QtCreator/Links/9a084b7ad885e9042acc2116e4ce17ab/.qt/QtDeploySupport.cmake)
include("${CMAKE_CURRENT_LIST_DIR}/stringlistmodelexample-plugins.cmake" OPTIONAL)
set(__QT_DEPLOY_I18N_CATALOGS "qtbase;qtdeclarative;qtdeclarative;qtdeclarative;qtdeclarative;qtdeclarative;qtdeclarative")

qt6_deploy_qml_imports(TARGET stringlistmodelexample PLUGINS_FOUND plugins_found)
qt6_deploy_runtime_dependencies(
    EXECUTABLE C:/ProgramData/QtCreator/Links/9a084b7ad885e9042acc2116e4ce17ab/stringlistmodelexample.exe
    ADDITIONAL_MODULES ${plugins_found}
    GENERATE_QT_CONF
)