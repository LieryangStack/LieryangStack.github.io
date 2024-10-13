#include <QGuiApplication>
#include <QStringList>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QStringListModel>
#include <QTimer>
#include <QDebug>


int main(int argc, char ** argv)
{
    QGuiApplication app(argc, argv);

    QStringList dataList = {
        "Item 1",
        "Item 2",
        "Item 3",
        "Item 4"
    };

    QQmlApplicationEngine engine;

    QStringListModel model;
    /* QStringListModel 内部拷贝了一份 dataList，所以操作原有的 dataList，并不会改变ListView显示内容 */
    model.setStringList(dataList);

    QObject::connect(&engine, &QQmlApplicationEngine::objectCreationFailed,
                     &app, []() { QCoreApplication::exit(-1); },
                     Qt::QueuedConnection);

    /* 仅仅对跟元素的属性起作用，我们很多时候都不是对根元素的属性设置,而且这是设置初始化属性值，也就是元素必须要有该属性（进行设置的属性，比如model） */
    // engine.setInitialProperties({{ "model", QVariant::fromValue(dataList)}});

    /* 另一种方式是将 QStringList 转换为 QVariantList，这是 QML 中可以直接使用的模型类型, 使用 QVariant::fromValue(dataList)  */
    engine.rootContext()->setContextProperty("model1", QVariant::fromValue(dataList));
    engine.rootContext()->setContextProperty("stringListModel", &model);

    engine.loadFromModule("Stringlistmodel", "View");

    /* 创建定时器 2s 后触发 */
    QTimer::singleShot(2000, [&]() {
        qDebug() << "2s";
        model.removeRows(0, 1);
        model.setData(model.index(0), "111");
    });


    return app.exec();
}

