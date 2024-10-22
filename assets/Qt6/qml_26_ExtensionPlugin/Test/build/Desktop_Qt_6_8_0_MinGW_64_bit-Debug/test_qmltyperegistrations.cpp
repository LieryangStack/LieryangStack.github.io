/****************************************************************************
** Generated QML type registration code
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <QtQml/qqml.h>
#include <QtQml/qqmlmoduleregistration.h>

#if __has_include(<myitem.h>)
#  include <myitem.h>
#endif


#if !defined(QT_STATIC)
#define Q_QMLTYPE_EXPORT Q_DECL_EXPORT
#else
#define Q_QMLTYPE_EXPORT
#endif
Q_QMLTYPE_EXPORT void qml_register_types_Test()
{
    QT_WARNING_PUSH QT_WARNING_DISABLE_DEPRECATED
    qmlRegisterTypesAndRevisions<MyItem>("Test", 1);
    qmlRegisterAnonymousType<QQuickItem, 254>("Test", 1);
    QT_WARNING_POP
    qmlRegisterModule("Test", 1, 0);
}

static const QQmlModuleRegistration testRegistration("Test", qml_register_types_Test);
