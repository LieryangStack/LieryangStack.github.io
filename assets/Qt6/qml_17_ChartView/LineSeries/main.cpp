#include <QApplication>
#include <QQmlApplicationEngine>


int main(int argc, char *argv[])
{
    /* QtCharts 使用 Qt Graphics View Framework 进行绘制，因此 QApplication一定要被使用，而不是 QGuiApplication */
    QApplication app(argc, argv);

    QQmlApplicationEngine engine;

    /* 如果 engine 在加载 QML 文件时遇到任何问题，例如 QML 文件中有语法错误，或者无法加载所需的组件，那么会发射 objectCreationFailed 信号。 */
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreationFailed,
                     &app, []() { QCoreApplication::exit(-1); },
    Qt::QueuedConnection);
    engine.loadFromModule("LineSeries", "Main");

    return app.exec();
}
