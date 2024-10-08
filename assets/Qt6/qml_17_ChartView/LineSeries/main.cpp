#include <QGuiApplication>
#include <QQmlApplicationEngine>


// int main(int argc, char *argv[])
// {
//     QGuiApplication app(argc, argv);

//     QQmlApplicationEngine engine;
//     QObject::connect(&engine, &QQmlApplicationEngine::objectCreationFailed,
//                      &app, []() { QCoreApplication::exit(-1); },
//     Qt::QueuedConnection);
//     engine.loadFromModule("LineSeries", "Main");

//     return app.exec();
// }


#include <QApplication>
#include <QDir>
#include <QQmlEngine>
#include <QQuickView>

int main(int argc, char *argv[])
{
    /* QtCharts 使用 Qt Graphics View Framework 进行绘制，因此 QApplication一定要被使用 */
    QApplication app(argc, argv);

//     QQuickView viewer;
//     viewer.setMinimumSize({600, 400});

//     // The following are needed to make examples run without having to install the module
//     // in desktop environments.
// #ifdef Q_OS_WIN
//     QString extraImportPath(QStringLiteral("%1/../../../../%2"));
// #else
//     QString extraImportPath(QStringLiteral("%1/../../../%2"));
// #endif
//     viewer.engine()->addImportPath(extraImportPath.arg(QGuiApplication::applicationDirPath(),
//                                                        QString::fromLatin1("qml")));
//     QObject::connect(viewer.engine(), &QQmlEngine::quit, &viewer, &QWindow::close);

//     viewer.setTitle(QStringLiteral("Qt Charts QML Example Gallery"));
//     viewer.setSource(QUrl("../../Main.qml"));
//     viewer.setResizeMode(QQuickView::SizeRootObjectToView);
//     viewer.show();

    QQmlApplicationEngine engine("../../Main.qml");

    return app.exec();
}

