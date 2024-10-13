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

    // 使用 QStringListModel
    QStringListModel model;
    model.setStringList(dataList);

    QStringList list = model.stringList();

    printf("dataList = %p\n", dataList);
    printf("dataList = %p\n", list);

    QObject::connect(&engine, &QQmlApplicationEngine::objectCreationFailed,
                     &app, []() { QCoreApplication::exit(-1); },
                     Qt::QueuedConnection);

    /* 另一种方式是将 QStringList 转换为 QVariantList，这是 QML 中可以直接使用的模型类型, 使用 QVariant::fromValue(dataList)  */

    /* 仅仅对跟元素的属性起作用 */
    // engine.setInitialProperties({{ "model1", QVariant::fromValue(dataList)}});


    engine.rootContext()->setContextProperty("model1", QVariant::fromValue(dataList));
    engine.rootContext()->setContextProperty("stringListModel", &model);
    engine.loadFromModule("Stringlistmodel", "View");

    // 创建定时器，5秒后添加 "猪" 到模型
    QTimer::singleShot(2000, [&]() {
        qDebug() << "2s";
        model.removeRows(0, 1);
        model.setData(model.index(0), "111");
    });


    return app.exec();
}

