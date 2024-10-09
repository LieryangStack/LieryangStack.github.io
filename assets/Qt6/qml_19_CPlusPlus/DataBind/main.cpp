#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QtQml>

#include "myhello.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    /* 注册 C++ 类型 MyHello */
    qmlRegisterType<MyHello>("MyHello", 1, 0, "MyHello");

    QQmlApplicationEngine engine;
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreationFailed,
                     &app, []() { QCoreApplication::exit(-1); },
    Qt::QueuedConnection);
    engine.loadFromModule("DataBind", "Main");

    return app.exec();
}
