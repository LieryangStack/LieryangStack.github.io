#include <QApplication>
#include <QQmlEngine>
#include <QQmlComponent>
#include <QQuickWindow>
#include <QQmlContext>


int main(int argc, char *argv[])
{
    /* 封装了所有与应用程序引用相关的属性（例如应用程序的名称、命令行参数、事件管理） */
    QApplication a(argc, argv);

    /* QQmlEngine类为实例化 QML 组件提供了环境，解析qml文件中的对象，创建root QQmlContext */
    QQmlEngine *engine = new QQmlEngine;

    /* qml文件路径 */
    const QUrl url(QStringLiteral("qrc:/main.qml"));

    /* 创建一个组件对象（用于加载和实例化单个 QML 组件。适合动态创建 QML 对象） */
    QQmlComponent component(engine, url);


    QObject *myObject = component.create();
    /* 可以再创建一个对象 */
    QObject *myObject1 = component.create();



    QQuickWindow *window = qobject_cast<QQuickWindow *>(myObject);

    /* 设定窗口对象的属性值 */
    window->setWidth(1000);
    window->setTitle("QML与C++交互");

    /* 修改自定义的属性值 */
    window->setProperty("buttonLabel", "按钮1");

    /* buttonLabel_Context 是在qml中直接绑定的，并且不能qml声明属性 */
    // QQmlContext *context = engine->rootContext();
    // context->setContextProperty("buttonLabel_Context", "测试按钮1");




    return a.exec();
}
