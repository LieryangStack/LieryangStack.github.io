---
layout: post
title: 十八、QML——QQmlApplicationEngine
categories: QML
tags: [QML]
---

- **QQmlContext**：负责 `QML` 和 `C++` 之间进行通信（属性绑定、数据传递）

- **QQmlEngine**：解析 `QML`文件的对象，便于C++实例化这些QML对象。管理 `QQmlContext` 上下文层次结构。

- **QQmlComponent**：能够创建（实例化）QML对象。

## 1 QQmlContext

![alt text](/assets/Qt6/qml_18_QQmlApplicationEngine/image/image-1.png)

1. `QQmlContext` 是 Qt 中的一个类，负责管理 QML 和 C++之间的上下文和数据通信。它运行将 C++对象、属性或数据传递给 QML 层，并在 QML 中使用这些数据进行绑定和操作。QQmlContext 在 QML 引擎中扮演了连接 QML 视图和 C++ 逻辑的桥梁作用。

    ```c
    /* main.qml */

    import QtQuick 2.15
    import QtQuick.Controls 2.15

    ApplicationWindow {
        visible: true
        width: 640
        height: 480
        /* 使用上下文中设置的 greetingText */
        title: greetingText  
    }

    /* main.cpp */

    #include <QApplication>
    #include <QQmlApplicationEngine>
    #include <QQmlContext>

    int main(int argc, char *argv[])
    {
        QApplication app(argc, argv);

        // 创建 QML 引擎
        QQmlApplicationEngine engine;

        // 获取根上下文
        QQmlContext *context = engine.rootContext();

        // 设置上下文属性
        QString text = "Hello, QML!";
        context->setContextProperty("greetingText", text);

        // 加载 QML 文件
        engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

        return app.exec();
    }
    ```

2. 上下文Contexts保存了QMl组件id所标识的对象。可以通过 `nameForObject()` 和 `objectForName()` 来检索这些对象。

3. 上下文可以不进行手动创建，创建 `QQmlEngine` 的时候，会创建一个默认的 `QQmlContext`。

## 2 QQmlEngine

![alt text](/assets/Qt6/qml_18_QQmlApplicationEngine/image/image-2.png)

1. 加载和解析 `QML` 文件。`QQmlEngine` 可以加载 QML 文件，将其解析成内部的 Qt 对象模型，并实例化对应的 QML 对象。这使得开发者能够使用 QML 描述用户界面的结构和行为，并由 QQmlEngine 负责将其转化为可执行的 UI 组件。

2. QQmlEngine类为实例化QML组件提供了环境。`QQmlEngine` 会自动创建 root context（QQmlContext）。每个QML组件都在 `QQmlContext` 中实例化。在QML中，上下文是按层次排列的，这次层次由QQmlEngine管理。

3. 在创建任何 QML 组件之前，应用程序必须创建一个 QQmlEngine 才能访问 QML 上下文。

## 3 QQmlComponent

![alt text](/assets/Qt6/qml_18_QQmlApplicationEngine/image/image-3.png)

1. `QQmlComponent` 组件是可重用的、封装的 QML 类型，具有定义良好的接口。调用 `create()` 函数，可以创建该组件的实例。

    可以从 QML 文件创建一个 QQmlComponent 实例。例如，如果有一个 main.qml 文件，如下所示：

    ```qml
    import QtQuick 2.0

    Item {
        width: 200
        height: 200
    }
    ```

    以下代码将该 QML 文件加载为一个组件，使用 create() 创建该组件的实例，然后查询 Item 的宽度值：

    ```c++
    QQmlEngine *engine = new QQmlEngine;
    QQmlComponent component(engine, QUrl::fromLocalFile("main.qml"));

    QObject *myObject = component.create();
    QQuickItem *item = qobject_cast<QQuickItem*>(myObject);
    int width = item->width();  // width = 200
    ```

## 4 QQmlApplicationEngine

- **QQmlComponent**： 用于加载和实例化单个 QML 组件。适合动态创建 QML 对象，但不适合管理整个应用的 QML 组件。

- **QQmlApplicationEngine**： QQmlEngine 的高级封装，专门用于应用程序级别的 QML 加载和管理。通常用于简单应用，可以直接加载主 QML 文件并自动管理根对象。

![alt text](/assets/Qt6/qml_18_QQmlApplicationEngine/image/image-4.png)

此类结合了 `QQmlEngine` 和 `QQmlComponent`，提供了一种方便的方法来加载单个 QML 文件。它还向 QML 暴露了一些核心应用程序功能，通常在 C++/QML 混合应用程序中由 C++ 控制。

可以这样使用：

```c++
#include <QGuiApplication>
#include <QQmlApplicationEngine>

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    QQmlApplicationEngine engine("main.qml");
    return app.exec();
}
```

与 `QQuickView` 不同，`QQmlApplicationEngine` 不会自动创建根窗口。如果你使用的是 Qt Quick 中的可视元素，你需要将它们放在一个 `Window` 内。

如果你没有使用需要 QGuiApplication 的 QML 模块（如 QtQuick），你也可以将 QCoreApplication 与 QQmlApplicationEngine 一起使用。

QQmlEngine 默认配置的更改列表：

- 将 Qt.quit() 连接到 QCoreApplication::quit()。

- 自动加载与主 QML 文件相邻的 i18n 目录中的翻译文件。翻译文件必须有 "qml_" 前缀，例如 qml_ja_JP.qm。

- 当 QJSEngine::uiLanguage / Qt.uiLanguage 属性更改时，翻译文件会重新加载。

- 如果场景包含 QQuickWindow，则自动设置一个孵化控制器（incubation controller）。

- 自动将 QQmlFileSelector 作为 URL 拦截器，应用文件选择器到所有 QML 文件和资源。

可以通过使用继承自 QQmlEngine 的方法进一步调整引擎行为。

## 4 QApplication

- **QCoreApplication**：是所有Qt程序的基础类，处理非图形用户界面的应用程序和主控制流和设置。适用于没有图形界面的应用程序。

- **QGuiApplication**：继承自 `QCoreApplication`，用于处理有图形用户界面的应用程序，但不涉及小部件`Widgets`。适用于不使用 `QWidget` 的GUI应用程序。

- **QApplication**：继承自 `QGuiApplication`，是所有涉及 `QWidget` 的应用程序的主类。适用于需要图形用户界面的小部件应用程序。


## 5 补充QStringLiteral

```c++
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
```
