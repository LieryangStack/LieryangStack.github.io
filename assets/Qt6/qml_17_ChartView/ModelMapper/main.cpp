#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QStandardItemModel>
#include <QDateTime>
#include <QTimer>
#include <QRandomGenerator>


int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreationFailed,
                     &app, []() { QCoreApplication::exit(-1); },
    Qt::QueuedConnection);

    /* 一百行数据，每行有三个元素 */
    QStandardItemModel lineModel(10, 3);
    int row = lineModel.rowCount();

    for (int row = 0; row < lineModel.rowCount(); row++) {
        QStandardItem *item1 = new QStandardItem(QString::number(row));
        QDateTime time = QDateTime::currentDateTime().addSecs(row);
        QStandardItem *item2 = new QStandardItem(QString::number(time.toMSecsSinceEpoch()));
        QStandardItem *item3 = new QStandardItem(QString::number(row*row));
        lineModel.setItem(row, 0, item1);
        lineModel.setItem(row, 1, item2);
        lineModel.setItem(row, 2, item3);
    }

    engine.rootContext()->setContextProperty("lineModel", &lineModel);
    engine.rootContext()->setContextProperty("row", 9);


    engine.loadFromModule("ModelMapper", "Main");

    QTimer timer;

    QObject::connect(&timer, &QTimer::timeout, [&]() {
        int row = lineModel.rowCount();
        QStandardItem *item1 = new QStandardItem(QString::number(row));
        QDateTime time = QDateTime::currentDateTime().addSecs(row);
        QStandardItem *item2 = new QStandardItem(QString::number(time.toMSecsSinceEpoch()));
        QStandardItem *item3 = new QStandardItem(QString::number(QRandomGenerator::global()->bounded(100)));
        lineModel.setItem(row, 0, item1);
        lineModel.setItem(row, 1, item2);
        lineModel.setItem(row, 2, item3);
    });

   timer.start(1000);

    return app.exec();
}
