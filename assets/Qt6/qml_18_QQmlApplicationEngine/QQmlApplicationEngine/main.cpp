#include <QApplication>
#include <QQmlApplicationEngine>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    /* QStringLiteral 创建的是字符串常量（常量内存段）
     * 如果字符串需要动态修改或不会频繁使用，那么就不适合使用 QStringLiteral，因为它只能用于常量字符串，且是只读的。
     * 总结来说，QStringLiteral 在处理常量字符串时能提供性能和内存的优化。
     */
    const QUrl url(QStringLiteral("qrc:/main.qml"));
    QQmlApplicationEngine engine(url);


    return a.exec();
}
