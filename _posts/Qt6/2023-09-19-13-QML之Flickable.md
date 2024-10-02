---
layout: post
title: 十三、QML——Flickable
categories: QML
tags: [QML]
---

Flickable 项目将其子项放置在一个可拖动和快速滑动的表面上Surface，从而使得视图可以滚动。这种行为构成了用于显示大量item项目（例如ListView和GridView）的基础。

在传统的用户界面中，可以使用标准控件（如滚动条和箭头按钮）来滚动视图。在某些情况下，也可以通过按住鼠标按钮并移动光标直接拖动视图。在基于触摸的用户界面中，这种拖动操作通常会伴随着快速滑动操作，即在用户停止触摸视图后，滚动会继续进行。

Flickable 不会自动裁剪其内容。如果它不是用作全屏项目，您应考虑将 clip 属性设置为 true。



