#include <QGuiApplication>
#include <QStringList>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QStringListModel>
#include <QTimer>
#include <QDebug>

#include "model.h"

int main(int argc, char ** argv)
{
    QGuiApplication app(argc, argv);

    AnimalModel model;
    model.addAnimal(Animal("Wolf", "Medium"));
    model.addAnimal(Animal("Polar bear", "Large"));
    model.addAnimal(Animal("Quoll", "Small"));

    // 创建定时器，1秒后添加 "猪" 到模型
    QTimer::singleShot(1000, [&]() {
        model.addAnimal(Animal("猪", "Large"));
        model.addAnimal(Animal("猪", "Large"));
        model.addAnimal(Animal("猪", "Large"));
        model.addAnimal(Animal("猪", "Large"));
    });

    QQmlApplicationEngine engine;
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreationFailed,
                     &app, []() { QCoreApplication::exit(-1); },
                     Qt::QueuedConnection);
    engine.rootContext()->setContextProperty("model1", QVariant::fromValue(&model));
    // engine.rootContext()->setContextProperty("model1", &model); /* 直接传入，或者 QVariant::fromValue(&model) 都可以  */

    engine.loadFromModule("abstractitemmodel", "View");

    return app.exec();
}

