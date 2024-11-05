---
layout: post
title: 十、QML——Component
categories: QML
tags: [QML]
---

1. `Component` 是能够可重用、封装好QML组件的一种类型。可以理解为，Component能够实例化（创建）QML组件。调用一次，实例化一次qml组件。

2. 每个qml对象，都能使用 `Component.completed()` 和 `Component.destruction()`

3. 这和使用C++实例化qml组件是类似的，只不过只是，qml文件内部有一次进行实例化另一个qml组件。

    ![alt text](/assets/Qt6/qml_10_Component/image/image.png)