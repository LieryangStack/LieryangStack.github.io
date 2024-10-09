#include <QApplication>
#include <QQmlApplicationEngine>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    const QUrl url(QStringLiteral("qrc:/main.qml"));
    QQmlApplicationEngine engine(url);
    return a.exec();
}
