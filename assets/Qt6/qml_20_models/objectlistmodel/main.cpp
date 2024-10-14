#include <QGuiApplication>
#include <QStringList>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QStringListModel>
#include <QTimer>
#include <QDebug>

#include "dataobject.h"
#include <QString>


int main(int argc, char ** argv)
{
    QGuiApplication app(argc, argv);

    const QStringList colorList = {"red",
                                   "green",
                                   "blue",
                                   "yellow"};

    const QStringList moduleList = {"Core", "GUI", "Multimedia", "Multimedia Widgets", "Network",
                                    "QML", "Quick", "Quick Controls", "Quick Dialogs",
                                    "Quick Layouts", "Quick Test", "SQL", "Widgets", "3D",
                                    "Android Extras", "Bluetooth", "Concurrent", "D-Bus",
                                    "Gamepad", "Graphical Effects", "Help", "Image Formats",
                                    "Location", "Mac Extras", "NFC", "OpenGL", "Platform Headers",
                                    "Positioning", "Print Support", "Purchasing", "Quick Extras",
                                    "Quick Timeline", "Quick Widgets", "Remote Objects", "Script",
                                    "SCXML", "Script Tools", "Sensors", "Serial Bus",
                                    "Serial Port", "Speech", "SVG", "UI Tools", "WebEngine",
                                    "WebSockets", "WebView", "Windows Extras", "XML",
                                    "XML Patterns", "Charts", "Network Authorization",
                                    "Virtual Keyboard", "Quick 3D", "Quick WebGL"};


    QList<QObject *> dataList;
    for (const QString &module : moduleList)
        dataList.append(new DataObject("Qt " + module, colorList.at(rand() % colorList.length())));

    QQmlApplicationEngine engine;
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreationFailed,
                     &app, []() { QCoreApplication::exit(-1); },
                     Qt::QueuedConnection);
    engine.rootContext()->setContextProperty("model1", QVariant::fromValue(dataList));

    engine.loadFromModule("objectlistmodel", "View");

    /* 创建定时器 1s 后触发 */
    QTimer::singleShot(1000, [&]() {
        dataList.insert(0, new DataObject("Qt Lieryang Lieryang Lieryang", "pink"));
        /* 需要重新设置 model1 属性，使得ListView重新设置 */
        engine.rootContext()->setContextProperty("model1", QVariant::fromValue(dataList));
    });

    return app.exec();
}
