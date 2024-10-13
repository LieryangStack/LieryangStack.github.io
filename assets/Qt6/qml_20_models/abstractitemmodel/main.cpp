#include "model.h"

#include <QGuiApplication>
#include <qqmlengine.h>
#include <qqmlcontext.h>
#include <qqml.h>
#include <QtQuick/qquickitem.h>
#include <QtQuick/qquickview.h>
#include <QTimer>


int main(int argc, char ** argv)
{
    QGuiApplication app(argc, argv);

    AnimalModel model;
    model.addAnimal(Animal("Wolf", "Medium"));
    model.addAnimal(Animal("Polar bear", "Large"));
    model.addAnimal(Animal("Quoll", "Small"));

    // 创建定时器，5秒后添加 "猪" 到模型
    QTimer::singleShot(2000, [&]() {
        model.addAnimal(Animal("猪", "Large"));
        model.addAnimal(Animal("猪", "Large"));
        model.addAnimal(Animal("猪", "Large"));
        model.addAnimal(Animal("猪", "Large"));
    });

    QQuickView view;
    view.setResizeMode(QQuickView::SizeRootObjectToView);
    view.setInitialProperties({{"model", QVariant::fromValue(&model)}});

    view.setSource(QUrl("qrc:/qt/qml/abstractitemmodel/view.qml"));
    view.show();

    return app.exec();
}

